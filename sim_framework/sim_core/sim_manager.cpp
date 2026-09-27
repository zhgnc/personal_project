// Implementation of the batch orchestrator. See sim_manager.hpp for the
// class contract and the order of the run() stages.

#include "sim_manager.hpp"

// Reads the simulation config and parses the app manifest it names. Parsing
// here means a broken manifest fails at construction rather than after the
// caller has set everything else up
SimManager::SimManager(const std::string& path_to_sim_config) {
    YAML::Node config = load_yaml_file(path_to_sim_config);

    start_time_sec       = get_yaml_value<double>(config, "sim_start_time_sec");
    stop_time_sec        = get_yaml_value<double>(config, "sim_stop_time_sec");
    sim_rate_hz          = get_yaml_value<double>(config, "simulation_rate_hz");
    num_mc_runs          = get_yaml_value<size_t>(config, "number_of_monte_carlo_runs");
    num_threads          = get_yaml_value<size_t>(config, "number_of_threads");
    init_seed            = get_yaml_value<uint64_t>(config, "initial_random_seed");

    print_hdf5_file_tree   = get_yaml_value<bool>(config, "print_hdf5_file_format");
    print_file_attributes  = get_yaml_value<bool>(config, "print_hdf5_attributes_in_file_format");
    sim_stats_report_level = static_cast<uint8_t>(get_yaml_value<int>(config, "sim_stats_report_level"));

    base_file_name       = get_yaml_value<std::string>(config, "base_file_name");
    output_directory     = get_yaml_value<std::string>(config, "logging_file_save_directory");

    app_count                = 0;
    io_connection_spec_count = 0;
    manifest_entry_count     = 0;
    app_class_factory_count  = 0;

    // The manifest defines which apps run; parsing it here means a broken
    // manifest fails at construction, before register_app_type()/run()
    std::string app_manifest_path = get_yaml_value<std::string>(config, "app_manifest");

    try {
        AppManifestParser manifest_parser(app_manifest_path);
        manifest_parser.parse(manifest_entries, manifest_entry_count);
    } catch (const IoWiringError& error) {
        std::cout << error.what() << std::endl;
        throw;
    }

    completed_run_count        = 0;
    reached_stop_time_count    = 0;
    early_stop_report_count    = 0;
    num_threads_used           = 0;
    total_sim_time_sec         = 0.0;
    total_run_wall_clock_sec   = 0.0;
    sim_to_real_time_ratio_sum = 0.0;
    fastest_run_wall_clock_sec = std::numeric_limits<double>::max();
    slowest_run_wall_clock_sec = 0.0;
    fastest_run_number         = 0;
    slowest_run_number         = 0;

    if (num_threads >= SimConfig::max_thread_number) {
        throw std::runtime_error(SimConfig::capacity_message("[sim_manager.cpp] `number_of_threads`",
                                                             "max_thread_number", SimConfig::max_thread_number));
    }
}

// Same setup as the config-only constructor, plus the project's list of app
// classes the manifest is allowed to instantiate
SimManager::SimManager(const std::string& path_to_sim_config, const AppClassList& available_apps)
    : SimManager(path_to_sim_config) {
    for (std::size_t i = 0; i < available_apps.count; i++) {
        add_class_factory(available_apps.factories[i]);
    }
}

void SimManager::add_class_factory(const AppClassFactory& factory) {
    if (app_class_factory_count >= SimConfig::max_app_number) {
        throw std::runtime_error(SimConfig::capacity_message("[sim_manager.cpp] number of listed app classes",
                                                             "max_app_number", SimConfig::max_app_number));
    }

    for (std::size_t i = 0; i < app_class_factory_count; i++) {
        if (app_class_factories[i].class_name == factory.class_name) {
            throw std::runtime_error("[sim_manager.cpp] App class '" + factory.class_name + "' is listed twice");
        }
    }

    app_class_factories[app_class_factory_count] = factory;
    app_class_factory_count = app_class_factory_count + 1;
}

// Creates a prototype for every enabled manifest entry via the registered
// class factories. Each app carries the priority its manifest entry declares
// (lower steps first); the parser guarantees those are unique.
void SimManager::instantiate_manifest_apps() {
    std::string missing_class_report;

    for (std::size_t i = 0; i < manifest_entry_count; i++) {
        const AppManifestEntry& entry = manifest_entries[i];

        if (entry.enabled == false) {
            continue;
        }

        const AppClassFactory* factory = nullptr;

        for (std::size_t k = 0; k < app_class_factory_count; k++) {
            if (app_class_factories[k].class_name == entry.class_name) {
                factory = &app_class_factories[k];
            }
        }

        if (factory == nullptr) {
            std::string registered_classes;
            std::string suggestion;
            std::size_t best_distance = std::max<std::size_t>(2, entry.class_name.size() / 3) + 1;

            for (std::size_t k = 0; k < app_class_factory_count; k++) {
                registered_classes += (k == 0 ? "" : ", ") + app_class_factories[k].class_name;

                std::size_t distance = IoRegistry::edit_distance(entry.class_name, app_class_factories[k].class_name);

                if (distance < best_distance) {
                    best_distance = distance;
                    suggestion    = app_class_factories[k].class_name;
                }
            }

            missing_class_report += "\n  - app '" + entry.instance_name + "': class '" + entry.class_name +
                                    "' is not available. Available classes: " +
                                    (registered_classes.empty() ? "(none)" : registered_classes) +
                                    (suggestion.empty() ? "" : "\n      did you mean '" + suggestion + "'?");
            continue;
        }

        if (app_count >= SimConfig::max_app_number) {
            throw std::runtime_error(SimConfig::capacity_message("[sim_manager.cpp] number of enabled apps",
                                                                 "max_app_number", SimConfig::max_app_number));
        }

        app_prototypes[app_count]     = factory->create_prototype(entry, entry.priority);
        app_prototypes[app_count].tlm = AppTlmSettings{entry.instance_name, entry.tlm_rate_hz, entry.tlm_level};

        app_count = app_count + 1;
    }

    if (missing_class_report.empty() == false) {
        std::string message = "[APP MANIFEST] app class(es) not in this project's AppClassList: add "
                              "app_class<ClassName>() to the list passed to SimManager" + missing_class_report;

        std::cout << message << std::endl;
        throw IoWiringError(message);
    }

    if (app_count == 0) {
        throw std::runtime_error("[sim_manager.cpp] No apps to run: the manifest has no enabled apps and none were added in code");
    }
}

// Reads each enabled app's config file: `input_mapping:` entries are
// appended to the stored specs and replayed into every run. Telemetry
// settings were already captured (manifest or add_app) and are copied
// parallel to the sorted prototypes for the recorder.
void SimManager::load_app_io_configs() {
    for (std::size_t i = 0; i < app_count; i++) {
        tlm_settings[i] = app_prototypes[i].tlm;

        if (app_prototypes[i].prototype->config_file().empty()) {
            continue;
        }

        try {
            ConnectionYamlParser parser(app_prototypes[i].prototype->name(),
                                        app_prototypes[i].prototype->config_file());

            parser.parse_input_mapping(io_connection_specs, io_connection_spec_count);
        } catch (const IoWiringError& error) {
            std::cout << error.what() << std::endl;
            throw;
        }
    }
}

// A connection naming a disabled app gets this specific message instead of
// the generic "app does not exist" the wiring resolver would produce
void SimManager::check_connections_against_disabled_apps() {
    std::string report;

    for (std::size_t i = 0; i < io_connection_spec_count; i++) {
        for (std::size_t k = 0; k < manifest_entry_count; k++) {
            bool disabled_source = manifest_entries[k].enabled == false &&
                                   manifest_entries[k].instance_name == io_connection_specs[i].from_app;

            if (disabled_source) {
                report += "\n  - " + io_connection_specs[i].destination_app + "." + io_connection_specs[i].input +
                          " <- " + io_connection_specs[i].from_app + "." + io_connection_specs[i].from_port +
                          "  (origin: " + io_connection_specs[i].origin + ")";
            }
        }
    }

    if (report.empty() == false) {
        std::string message = "[APP MANIFEST] connection(s) reference apps that are disabled in the manifest "
                              "(enabled: false):" + report;

        std::cout << message << std::endl;
        throw IoWiringError(message);
    }
}

// Builds a throwaway registry from the app prototypes and resolves the full
// topology once, before any threads are spawned or output files created, so
// wiring errors print exactly once and abort the batch cleanly. Every run
// still resolves its own registry against its cloned apps afterwards.
void SimManager::validate_io_wiring() {
    auto validation_registry = std::make_unique<IoRegistry>();

    for (std::size_t i = 0; i < app_count; i++) {
        validation_registry->begin_declarations(app_prototypes[i].prototype->name());
        app_prototypes[i].prototype->declare_io(*validation_registry);
        validation_registry->end_declarations();
    }

    for (std::size_t i = 0; i < io_connection_spec_count; i++) {
        validation_registry->connect(io_connection_specs[i].destination_app,
                                     io_connection_specs[i].input,
                                     io_connection_specs[i].from_app,
                                     io_connection_specs[i].from_port,
                                     io_connection_specs[i].origin);
    }

    try {
        validation_registry->resolve_and_validate();
    } catch (const IoWiringError& error) {
        std::cout << error.what() << std::endl;
        throw;
    }

    std::cout << validation_registry->io_report() << "\n";
}

// Stage order here is load-bearing: apps must exist before they can be
// sorted, sorted before their configs are read into the parallel
// tlm_settings array, and the whole topology must validate before any thread
// starts or any output file is created
void SimManager::run() {
    instantiate_manifest_apps();

    sort_apps_by_priority(); // SimSingleRun expects the apps to be in the proper order when received in the config struct

    load_app_io_configs();
    check_connections_against_disabled_apps();
    validate_io_wiring();

    std::array<std::jthread, SimConfig::max_thread_number> thread_pool;

    std::size_t max_cpu_threads = std::thread::hardware_concurrency();
    max_cpu_threads             = std::max({max_cpu_threads, std::size_t{1}}); // Protects against `hardware_concurrency()` returning 0
    num_threads_used            = std::min({num_threads, max_cpu_threads});    // Protects against user inputting more threads than possible

    current_run_number = 1; // SimManager configured to use 1 based indexing for run numbers

    std::chrono::high_resolution_clock::time_point parallel_start_time = std::chrono::high_resolution_clock::now();

    for (std::size_t i = 0; i < num_threads_used; i++) {
        thread_pool[i] = std::jthread(&SimManager::thread_job, this);
    }

    for (std::size_t i = 0; i < num_threads_used; i++) {
        thread_pool[i].join();
    }

    std::chrono::duration<double> parallel_elapsed_seconds = std::chrono::high_resolution_clock::now() - parallel_start_time;

    display_run_summary(parallel_elapsed_seconds.count());
}

// Orders the prototypes by the priority each manifest entry declared. The
// manifest parser has already rejected duplicate priorities among enabled
// apps, so this order is unambiguous
void SimManager::sort_apps_by_priority() {
  std::cout << "[SimManager] Configuring Simulation\n";
  std::sort(app_prototypes.begin(), app_prototypes.begin() + app_count, SimManager::compare_by_priority);

  display_sorted_app_info();
};

bool SimManager::compare_by_priority(const SimAppPrototype& app_a, const SimAppPrototype& app_b) {
  // `<` sorts the apps to execute in ascending order (lower numbers run earlier)
  return app_a.prototype->priority() < app_b.prototype->priority();
};

void SimManager::display_sorted_app_info() {
  std::cout << "[SimManager] Sorted Application List\n";

  for (std::size_t i = 0; i < app_count; i++) {
    std::cout << "App Name: " << app_prototypes[i].prototype->name()
              << " | Priority: " << app_prototypes[i].prototype->priority()
              << " | Time Step (s): " << app_prototypes[i].prototype->dt_sec() << '\n';
  }

  std::cout << "\n";
}

// Worker function executed by each thread in the pool. Each worker claims
// the next available Monte Carlo run, builds an independent SimSingleRun,
// executes it, and then returns for another run until no work remains.
// Only claiming a run number and recording its stats touch shared state
void SimManager::thread_job() {
    while (true) {
        SimSingleRunConfig single_run_config;

        { // Used to scope std::lock_guard
        std::lock_guard<std::mutex> lock(mutex);

        if (current_run_number > num_mc_runs ) {
            return;
        }

        single_run_config  = build_single_run_config(current_run_number);
        current_run_number = current_run_number + 1;

        } // mutex is automatically unlocked here because std::lock_guard goes out of scope

        SimSingleRun single_run(std::move(single_run_config));
        SimRunStats run_stats = single_run.run();

        record_run_stats(run_stats);
    }
}

// Folds one finished run into the batch totals. Called from worker threads,
// so every member it touches is written under the lock
void SimManager::record_run_stats(const SimRunStats& run_stats) {
    std::lock_guard<std::mutex> lock(mutex);

    completed_run_count        = completed_run_count        + 1;
    total_sim_time_sec         = total_sim_time_sec         + run_stats.sim_time_sec;
    total_run_wall_clock_sec   = total_run_wall_clock_sec   + run_stats.wall_clock_sec;
    sim_to_real_time_ratio_sum = sim_to_real_time_ratio_sum + run_stats.sim_to_real_time_ratio;

    if (run_stats.stop_type == StopType::NoStop) {
        reached_stop_time_count += 1;
    } else if (early_stop_report_count < SimConfig::max_app_number) {
        early_stop_reports[early_stop_report_count] = "Run #" + std::to_string(run_stats.run_number)
                                                    + " stopped early: " + stop_reason_to_string(run_stats.stop_reason)
                                                    + " (" + run_stats.stop_message + ")";
        early_stop_report_count = early_stop_report_count + 1;
    }

    if (run_stats.wall_clock_sec < fastest_run_wall_clock_sec) {
        fastest_run_wall_clock_sec = run_stats.wall_clock_sec;
        fastest_run_number         = run_stats.run_number;
    }

    if (run_stats.wall_clock_sec > slowest_run_wall_clock_sec) {
        slowest_run_wall_clock_sec = run_stats.wall_clock_sec;
        slowest_run_number         = run_stats.run_number;
    }
}

// Prints the end-of-batch summary at the configured `sim_stats_report_level`:
//      Level 1 prints the completion message only
//      Level 2 adds performance stats
//      Level 3 or greater adds the run outcome breakdown and other details
void SimManager::display_run_summary(double parallel_wall_clock_sec) {
    if (completed_run_count < 1) {
        std::cout << "\n\n[SimManager] Simulation completed but no runs were completed!!!\n";
        return;
    }

    std::cout << "\n[SimManager] Simulation complete!!!\n";
    std::cout << "  |- Results save here: " << output_directory << "\n\n";

    if (sim_stats_report_level <= 1) {
        return;
    }

    double average_run_speed = sim_to_real_time_ratio_sum / completed_run_count;

    double time_saved_sec     = 0.0;
    double parallel_run_speed = 0.0;
    double parallel_speedup   = 0.0;

    if (num_threads == 1) {
        total_run_wall_clock_sec = parallel_wall_clock_sec;
    }

    if (parallel_wall_clock_sec > 0.0 && num_threads > 1) {
        parallel_run_speed = total_sim_time_sec       / parallel_wall_clock_sec;
        parallel_speedup   = total_run_wall_clock_sec / parallel_wall_clock_sec;
        time_saved_sec     = total_run_wall_clock_sec - parallel_wall_clock_sec;
    }

    std::cout << "[SimManager] Summary Info\n";
    std::cout << "  |- Monte Carlo runs completed:        "  << completed_run_count << " of " << num_mc_runs << "\n";
    std::cout << "  |- Total sim time for all runs:       "  << total_sim_time_sec << " sec\n";
    std::cout << "  |- Total wall clock time:             "  << total_run_wall_clock_sec << " sec\n";
    std::cout << "  |- Parallel wall clock time:          "  << parallel_wall_clock_sec << " sec\n";
    std::cout << "  |- Average individual run speed:      x" << average_run_speed << " faster than real time\n";
    std::cout << "  |- Parallel processing speed:         x" << parallel_run_speed << " faster than real time\n";
    std::cout << "  |- Time saved by parallelization:     "  << time_saved_sec << " sec (x" << parallel_speedup << " faster) on " << num_threads_used << " threads\n\n";

    if (sim_stats_report_level <= 2) {
        return;
    }

    if (completed_run_count > 0) {
        std::cout << "  |- Fastest run:                       #" << fastest_run_number << " (" << fastest_run_wall_clock_sec << " sec)\n";
        std::cout << "  |- Slowest run:                       #" << slowest_run_number << " (" << slowest_run_wall_clock_sec << " sec)\n";
    }

    std::size_t early_stopped_count = completed_run_count - reached_stop_time_count;

    std::cout << "  |- # Runs to reach config stop time:  " << reached_stop_time_count << "\n";
    std::cout << "  |- # Runs stopped early by an app:    " << early_stopped_count << "\n";

    for (std::size_t i = 0; i < early_stop_report_count; i++) {
        std::cout << "      |- " << early_stop_reports[i] << "\n";
    }

    if (early_stopped_count > early_stop_report_count) {
        std::cout << "      |- (only the first " << early_stop_report_count << " early-stop reports are stored)\n";
    }
}

// Produces a self-contained description of one run: fresh app clones plus
// copies of the shared config. Nothing in the returned struct aliases
// SimManager state, which is what lets runs execute in parallel
SimSingleRunConfig SimManager::build_single_run_config(std::size_t run_number) {
    SimSingleRunConfig run_config;

    run_config.start_time_sec        = start_time_sec;
    run_config.stop_time_sec         = stop_time_sec;
    run_config.sim_rate_hz           = sim_rate_hz;
    run_config.base_file_name        = base_file_name;
    run_config.output_directory      = output_directory;
    run_config.run_number            = run_number;
    run_config.print_hdf5_file_tree  = print_hdf5_file_tree;
    run_config.print_file_attributes = print_file_attributes;
    run_config.total_mc_runs         = num_mc_runs;
    run_config.initial_random_seed   = init_seed;

    run_config.app_count = app_count;

    for (std::size_t i = 0; i < app_count; i++) {
        run_config.apps[i] = app_prototypes[i].create();
        run_config.tlm_settings[i] = tlm_settings[i];
    }

    run_config.io_connection_count = io_connection_spec_count;

    for (std::size_t i = 0; i < io_connection_spec_count; i++) {
        run_config.io_connections[i] = io_connection_specs[i];
    }

    return run_config;
}
