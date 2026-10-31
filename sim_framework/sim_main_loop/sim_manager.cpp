#include "sim_manager.hpp"

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
    print_io_report        = get_yaml_value<bool>(config, "print_io_report");
    sim_stats_report_level = static_cast<uint8_t>(get_yaml_value<int>(config, "sim_stats_report_level"));

    base_file_name       = get_yaml_value<std::string>(config, "base_file_name");
    output_directory     = get_yaml_value<std::string>(config, "logging_file_save_directory");

    app_count = 0;

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

    bool too_many_threads = num_threads > SimConfig::max_thread_number;

    if (too_many_threads == true) {
        throw std::runtime_error(SimConfig::capacity_message("[SimManager] `number_of_threads`",
                                                             "max_thread_number", SimConfig::max_thread_number));
    }
}

void SimManager::run() {
    sort_apps_by_priority(); // SimSingleRun expects the apps to be in the proper order when received in the config struct
    validate_apps();

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

void SimManager::sort_apps_by_priority() {
  std::cout << "[SimManager] Configuring Simulation\n";
  std::sort(app_prototypes.begin(), app_prototypes.begin() + app_count, SimManager::compare_by_priority);
  
  display_sorted_app_info();
};

bool SimManager::compare_by_priority(const SimAppPrototype& app_A, const SimAppPrototype& app_B) {
  // `<` sorts the apps to execute in ascending order (lower numbers run earlier) 
  return app_A.prototype->priority() < app_B.prototype->priority();
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

// Checks every app once on the prototypes before any thread starts, so a bad
// rate or wiring mistake stops the batch with one error instead of failing in
// every worker thread
void SimManager::validate_apps() {
    for (std::size_t i = 0; i < app_count; i++) {
        AppRunner(*app_prototypes[i].prototype).check_rate(sim_rate_hz);
    }

    auto validation_registry = std::make_unique<IoRegistry>();

    for (std::size_t i = 0; i < app_count; i++) {
        validation_registry->add_app(app_prototypes[i].prototype->name());

        IoDeclarations io(*validation_registry);
        app_prototypes[i].prototype->declare_io(io); // User defined code
    }

    validation_registry->resolve_and_validate();

    if (print_io_report == true) {
        std::cout << validation_registry->io_report() << "\n";
    }
}

// Worker function executed by each thread in the pool. Each worker claims 
// the next available Monte Carlo run, builds an independent SimSingleRun, 
// executes it, and then returns for another run until no work remains.
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
        run_config.apps[i]         = app_prototypes[i].create();
        run_config.tlm_settings[i] = app_prototypes[i].tlm;
    }

    return run_config;
}