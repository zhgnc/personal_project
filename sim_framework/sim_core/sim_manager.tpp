#include "sim_manager.hpp"

template<typename DataBusType>
SimManager<DataBusType>::SimManager(const std::string& path_to_sim_config, DataBusType& bus) : data_bus(bus) {
    YAML::Node config = load_yaml_file(path_to_sim_config);

    start_time_sec       = get_yaml_value<double>(config, "sim_start_time_sec");
    stop_time_sec        = get_yaml_value<double>(config, "sim_stop_time_sec");
    sim_rate_hz          = get_yaml_value<double>(config, "simulation_rate_hz");
    num_mc_runs          = get_yaml_value<size_t>(config, "number_of_monte_carlo_runs");
    init_seed            = get_yaml_value<uint64_t>(config, "initial_random_seed");
    
    print_hdf5_file_tree  = get_yaml_value<bool>(config_data, "print_hdf5_file_format");
    print_file_attributes = get_yaml_value<bool>(config_data, "print_hdf5_attributes_in_file_format");

    base_file_name       = get_yaml_value<std::string>(config, "base_file_name");
    output_directory     = get_yaml_value<std::string>(config, "logging_file_save_directory");

    app_count         = 0;
    logging_app_count = 0;

    logging_rates.rate_A_hz = get_yaml_value<double>(config_data, "logging_rate_A_hz");
    logging_rates.rate_B_hz = get_yaml_value<double>(config_data, "logging_rate_B_hz");
    logging_rates.rate_C_hz = get_yaml_value<double>(config_data, "logging_rate_C_hz");
    logging_rates.rate_D_hz = get_yaml_value<double>(config_data, "logging_rate_D_hz");
    logging_rates.rate_E_hz = get_yaml_value<double>(config_data, "logging_rate_E_hz");
}

// This function accepts any object derived from `SimAppBase<DataBusType>` and 
// adds it to the `app_list`. The template parameter `AppType` preserves the 
// derived type so the sim can later create copies of the derived type for
// each simulation run.
template<typename DataBusType>
template<typename AppType>
void SimManager<DataBusType>::add_app(AppType&& new_app) {
    static_assert(std::is_base_of_v<SimAppBase<DataBusType>, std::decay_t<AppType>>,
                 "AppType must derive from SimAppBase<DataBusType>");
    
    if (app_count >= SimConfig::max_app_number) {
        throw std::runtime_error("[Simulation.tpp] Number of apps exceeded value of `max_app_number` in sim_config.hpp which is "
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
        throw std::runtime_error("[Simulation.tpp] Number of logging apps exceeded value of `max_logging_app_number` in sim_config.hpp which is"
                                + std::to_string(SimConfig::max_logging_app_number));
    }

    logging_prototypes[logging_app_count] = LoggingAppPrototype<DataBusType>(std::forward<LoggingAppType>(new_logging_app));
    logging_app_count = logging_app_count + 1;
}

template<typename DataBusType>
void SimManager<DataBusType>::run() {
    sort_apps_by_priority();

    // TODO: Make for loop run in parallel

    // For loop configured so that run_num starts at 1
    for (std::size_t run_num = 1; run_num < num_mc_runs + 1; run_num++) {
        SimSingleRunConfig<DataBusType> single_run_config = build_single_run_config(run_num);

        // Using `std::move()` to transfer ownership of `single_run_config` to `single_run`
        SimSingleRun<DataBusType> single_run(std::move(single_run_config)); 

        single_run.run();
    }
}

template<typename DataBusType>
void SimManager<DataBusType>::sort_apps_by_priority() {
  std::cout << "[SimManager] Configuring Simulation\n";
  std::sort(app_list.begin(), app_list.begin() + app_count, Simulation::compare_by_priority);
  
  display_sorted_app_info();
};

template<typename DataBusType>
bool SimManager<DataBusType>::compare_by_priority(const std::shared_ptr<SimAppBase<DataBusType>> &app_A,
                                     const std::shared_ptr<SimAppBase<DataBusType>> &app_B) {
  // `<` sorts the apps to execute in ascending order (lower numbers run earlier) 
  return app_A->priority() < app_B->priority();
};

template<typename DataBusType>
void SimManager<DataBusType>::display_sorted_app_info() {
  std::cout << "[SimManager] Sorted Application List\n";

  for (std::size_t i = 0; i < app_count; i++) {
    std::cout << "App Name: " << app_list[i]->name()
              << " | Priority: " << app_list[i]->priority()
              << " | Time Step (s): " << app_list[i]->dt_sec() << '\n';
  }

  std::cout << "\n";
}

template<typename DataBusType>
SimSingleRunConfig<DataBusType> SimManager<DataBusType>::build_single_run_config(const std::size_t& run_number) {    
    SimSingleRunConfig<DataBusType> run_config;
    
    run_config.data_bus              = data_bus;
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