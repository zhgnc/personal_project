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
    
    print_hdf5_file_tree  = get_yaml_value<bool>(config, "print_hdf5_file_format");
    print_file_attributes = get_yaml_value<bool>(config, "print_hdf5_attributes_in_file_format");

    base_file_name       = get_yaml_value<std::string>(config, "base_file_name");
    output_directory     = get_yaml_value<std::string>(config, "logging_file_save_directory");

    app_count         = 0;
    logging_app_count = 0;

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

    std::size_t max_cpu_threads  = std::thread::hardware_concurrency();
    max_cpu_threads              = std::max({max_cpu_threads, std::size_t{1}}); // Protects against `hardware_concurrency()` returning 0
    std::size_t num_threads_used = std::min({num_threads, max_cpu_threads});    // Protects against user inputting more threads than possible

    current_run_number = 1; // SimManager configured to use 1 based indexing for run numbers

    for (std::size_t i = 0; i < num_threads_used; i++) {
        thread_pool[i] = std::jthread(&SimManager::thread_job, this);
    }

    for (std::size_t i = 0; i < num_threads_used; i++) {
        thread_pool[i].join();
    }

    std::cout << "\n[SimManager] Simulation complete!!!\n\n";
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
        single_run.run();
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