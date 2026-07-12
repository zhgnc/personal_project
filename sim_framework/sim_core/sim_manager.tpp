#include "sim_manager.hpp"

template<typename DataBusType>
SimManager<DataBusType>::SimManager(const std::string& path_to_sim_config, DataBusType& bus) : data_bus(bus) {
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

    app_count         = 0;
    logging_app_count = 0;

    completed_run_count        = 0;
    reached_stop_time_count    = 0;
    early_stop_report_count    = 0;
    num_threads_used           = 0;
    total_sim_time_sec         = 0.0;
    total_run_wall_clock_sec   = 0.0;
    total_ratio_sum            = 0.0;
    fastest_run_wall_clock_sec = std::numeric_limits<double>::max();
    slowest_run_wall_clock_sec = 0.0;
    fastest_run_number         = 0;
    slowest_run_number         = 0;

    logging_rates.rate_A_hz = get_yaml_value<double>(config, "logging_rate_A_hz");
    logging_rates.rate_B_hz = get_yaml_value<double>(config, "logging_rate_B_hz");
    logging_rates.rate_C_hz = get_yaml_value<double>(config, "logging_rate_C_hz");
    logging_rates.rate_D_hz = get_yaml_value<double>(config, "logging_rate_D_hz");
    logging_rates.rate_E_hz = get_yaml_value<double>(config, "logging_rate_E_hz");

    if (num_threads >= SimConfig::max_thread_number) {
        throw std::runtime_error("[sim_manager.tpp] `number_of_threads` must not exceeded value of `max_thread_number` in sim_config.hpp which is"
                                + std::to_string(SimConfig::max_thread_number));
    }
}

// This function accepts any object derived from `SimAppBase<DataBusType>` and 
// adds it to the `app_prototypes`. The template parameter `AppType` preserves 
// the derived type so the sim can later create copies of the derived type for
// each simulation run.
template<typename DataBusType>
template<typename AppType>
void SimManager<DataBusType>::add_app(AppType&& new_app) {
    static_assert(std::is_base_of_v<SimAppBase<DataBusType>, std::decay_t<AppType>>,
                 "AppType must derive from SimAppBase<DataBusType>");
    
    if (app_count >= SimConfig::max_app_number) {
        throw std::runtime_error("[sim_manager.tpp] Number of apps exceeded value of `max_app_number` in sim_config.hpp which is "
                                + std::to_string(SimConfig::max_app_number));
    }

    // Line wrapps the `new_app` in a SimAppPrototype struct, which stores the original app 
    // instance along with a function that can create independent copies for each MC run. 
    // This is necessary because apps are stored polymorphically as `SimAppBase` pointers, 
    // and C++ cannot automatically copy the original derived type through a base-class 
    // pointer. `std::forward` preserves whether the app was passed as an lvalue or rvalue, 
    // avoiding unnecessary copies while constructing the prototype.
    app_prototypes[app_count] = SimAppPrototype<DataBusType>(std::forward<AppType>(new_app));
    app_count = app_count + 1;
}

// Function contains the ability to add logging apps to the simulation and see comments for 
// `add_app` to underrstand why the code is written the way it is
template<typename DataBusType>
template<typename LoggingAppType>
void SimManager<DataBusType>::add_logging_app(LoggingAppType&& new_logging_app) {
    static_assert(std::is_base_of_v<LoggingAppBase<DataBusType>, std::decay_t<LoggingAppType>>,
                 "LoggingAppType must derive from LoggingAppBase<DataBusType>");

    if (logging_app_count >= SimConfig::max_logging_app_number) {
        throw std::runtime_error("[sim_manager.tpp] Number of logging apps exceeded value of `max_logging_app_number` in sim_config.hpp which is"
                                + std::to_string(SimConfig::max_logging_app_number));
    }

    logging_prototypes[logging_app_count] = LoggingAppPrototype<DataBusType>(std::forward<LoggingAppType>(new_logging_app));
    logging_app_count = logging_app_count + 1;
}

template<typename DataBusType>
void SimManager<DataBusType>::run() {
    sort_apps_by_priority(); // SimSingleRun expects the apps to be in the proper order when received in the config struct

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

template<typename DataBusType>
void SimManager<DataBusType>::sort_apps_by_priority() {
  std::cout << "[SimManager] Configuring Simulation\n";
  std::sort(app_prototypes.begin(), app_prototypes.begin() + app_count, SimManager<DataBusType>::compare_by_priority);
  
  display_sorted_app_info();
};

template<typename DataBusType>
bool SimManager<DataBusType>::compare_by_priority(const SimAppPrototype<DataBusType>& app_A, 
                                                  const SimAppPrototype<DataBusType>& app_B) {
  // `<` sorts the apps to execute in ascending order (lower numbers run earlier) 
  return app_A.prototype->priority() < app_B.prototype->priority();
};

template<typename DataBusType>
void SimManager<DataBusType>::display_sorted_app_info() {
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
template<typename DataBusType>
void SimManager<DataBusType>::thread_job() {
    while (true) {
        SimSingleRunConfig<DataBusType> single_run_config;

        { // Used to scope std::lock_guard
        std::lock_guard<std::mutex> lock(mutex);

        if (current_run_number > num_mc_runs ) {
            return;
        }

        single_run_config  = build_single_run_config(current_run_number);
        current_run_number = current_run_number + 1;

        } // mutex is automatically unlocked here because std::lock_guard goes out of scope

        SimSingleRun<DataBusType> single_run(std::move(single_run_config));
        SimRunStats run_stats = single_run.run();

        record_run_stats(run_stats);
    }
}

template<typename DataBusType>
void SimManager<DataBusType>::record_run_stats(const SimRunStats& run_stats) {
    std::lock_guard<std::mutex> lock(mutex);

    completed_run_count      = completed_run_count      + 1;
    total_sim_time_sec       = total_sim_time_sec       + run_stats.sim_time_sec;
    total_run_wall_clock_sec = total_run_wall_clock_sec + run_stats.wall_clock_sec;
    total_ratio_sum          = total_ratio_sum          + run_stats.sim_to_real_time_ratio;

    if (run_stats.stop_type == StopType::NoStop) {
        reached_stop_time_count += 1;
    } else if (early_stop_report_count < SimConfig::max_app_number) {
        early_stop_reports[early_stop_report_count] = "Run #" + std::to_string(run_stats.run_number)
                                                    + " stopped early: " + stop_reason_to_string(run_stats.stop_reason)
                                                    + " (" + run_stats.stop_message + ")";
        early_stop_report_count += 1;
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
template<typename DataBusType>
void SimManager<DataBusType>::display_run_summary(double parallel_wall_clock_sec) {
    std::cout << "\n[SimManager] Simulation complete!!!\n";
    std::cout << "  |- Results save here: " << output_directory << "\n\n";

    if (sim_stats_report_level <= 1) {
        std::cout << "\n";
        return;
    }

    double time_saved_sec     = 0.0;
    double average_run_speed  = 0.0;
    double parallel_run_speed = 0.0;
    double parallel_speedup   = 0.0;

    if (completed_run_count > 0) {
        average_run_speed = total_ratio_sum / completed_run_count;
    }

    if (num_threads == 1) {
        total_run_wall_clock_sec = parallel_wall_clock_sec;
    }

    if (parallel_wall_clock_sec > 0.0 && num_threads > 1) {
        parallel_run_speed = total_sim_time_sec / parallel_wall_clock_sec;
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
        std::cout << "\n";
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

template<typename DataBusType>
SimSingleRunConfig<DataBusType> SimManager<DataBusType>::build_single_run_config(std::size_t run_number) {
    SimSingleRunConfig<DataBusType> run_config;
    
    run_config.data_bus              = DataBusType(data_bus);
    run_config.start_time_sec        = start_time_sec;
    run_config.stop_time_sec         = stop_time_sec;
    run_config.sim_rate_hz           = sim_rate_hz;
    run_config.base_file_name        = base_file_name;
    run_config.output_directory      = output_directory;
    run_config.run_number            = run_number;
    run_config.logging_rates         = logging_rates;
    run_config.print_hdf5_file_tree  = print_hdf5_file_tree;
    run_config.print_file_attributes = print_file_attributes;
    run_config.total_mc_runs         = num_mc_runs;
    run_config.initial_random_seed   = init_seed;

    run_config.app_count = app_count;

    for (std::size_t i = 0; i < app_count; i++) {
        run_config.apps[i] = app_prototypes[i].create();
    }

    run_config.logging_app_count = logging_app_count;

    for (std::size_t i = 0; i < logging_app_count; i++) {
        run_config.logging_apps[i] = logging_prototypes[i].create();
    }

    return run_config;
}