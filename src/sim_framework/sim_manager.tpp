#include "sim_manager.hpp"

template<typename DataBusType>
SimManager<DataBusType>::SimManager(const std::string &path_to_sim_config, DataBusType& bus) 
  : data_bus(bus)
{
  YAML::Node config_data = load_yaml_file(path_to_sim_config);

  start_time_sec          = get_yaml_key<double>(config_data, "sim_start_time_sec");
  config_stop_time_sec    = get_yaml_key<double>(config_data, "sim_stop_time_sec");
  sim_rate_hz             = get_yaml_key<double>(config_data, "simulation_rate_hz");
  num_mc_runs             = get_yaml_key<std::size_t>(config_data, "number_of_monte_carlo_runs");
  num_parallel_processors = get_yaml_key<std::size_t>(config_data,  "number_of_parallel_processors");
  init_seed               = get_yaml_key<int>(config_data,  "initial_random_seed");
  print_hdf5_file_tree    = get_yaml_key<bool>(config_data, "print_hdf5_file_format");
  print_file_attributes   = get_yaml_key<bool>(config_data, "print_hdf5_attributes_in_file_format");

  data_output_directory   = get_yaml_key<std::string>(config_data, "logging_file_save_directory");
  base_file_name          = get_yaml_key<std::string>(config_data, "logging_filename_prefix");
  logging_rates.rate_A_hz = get_yaml_key<double>(config_data, "logging_rate_A_hz");
  logging_rates.rate_B_hz = get_yaml_key<double>(config_data, "logging_rate_B_hz");
  logging_rates.rate_C_hz = get_yaml_key<double>(config_data, "logging_rate_C_hz");
  logging_rates.rate_D_hz = get_yaml_key<double>(config_data, "logging_rate_D_hz");
  logging_rates.rate_E_hz = get_yaml_key<double>(config_data, "logging_rate_E_hz");

  sim_dt_usec           = static_cast<uint64_t>(sec2usec * (1.0 / sim_rate_hz));
  current_sim_time_usec = static_cast<uint64_t>(sec2usec * start_time_sec);
  current_sim_time_sec  = start_time_sec;
  config_stop_time_usec = static_cast<uint64_t>(sec2usec * config_stop_time_sec);
};

// This function accepts an object derived from `SimAppBase<DataBusType>` and 
// adds it to the `app_list`. A template AppType is used to preserve the virtual 
// functions that the `new_app` overwrote because if the input type was 
// `SimAppBase<DataBusType>` then the `new_app` specific behavior would be lost.
template<typename DataBusType>
template<typename AppType>
void SimManager<DataBusType>::add_app(AppType&& new_app) {
  static_assert(std::is_base_of_v<SimAppBase<DataBusType>, std::decay_t<AppType>>,
                "AppType must derive from SimAppBase<DataBusType>");
    
  // The `push_back` function uses `std::decay_t<AppType>` to ensure that the 
  // shared_ptr stores the actual derived app rather than base type. Then 
  // `std::forward<AppType>(app)` is used to allow flexibility of passing 
  // `new_app` as an lvalue or rvalue. Simply `new_app` is added to the `app_list`
  app_list.push_back(std::make_shared<std::decay_t<AppType>>(std::forward<AppType>(new_app)));
}

// See comments for `add_app()` to understand why templates, `decay_t`, and `forward`
template<typename DataBusType>
template<typename LoggingAppType>
void SimManager<DataBusType>::add_logging_app(LoggingAppType&& new_logging_app) {
    static_assert(std::is_base_of_v<LoggingAppBase<DataBusType>, std::decay_t<LoggingAppType>>,
                  "LoggingAppType must derive from LoggingAppBase<DataBusType>");

    logging_apps.push_back(std::make_shared<std::decay_t<LoggingAppType>>(std::forward<LoggingAppType>(new_logging_app)));
}

template<typename DataBusType>
void SimManager<DataBusType>::sort_apps_by_priority() {
  std::cout << "[Sim Manager] Configuring Simulation\n";
  std::sort(app_list.begin(), app_list.end(), SimManager::compare_by_priority);

  display_sorted_app_info();
};

template<typename DataBusType>
bool SimManager<DataBusType>::compare_by_priority(const std::shared_ptr<SimAppBase<DataBusType>> &app_A,
                                     const std::shared_ptr<SimAppBase<DataBusType>> &app_B) {
  // `<` sorts the apps to execute in ascending order (lower numbers run earlier) 
  return app_A->priority < app_B->priority;
};

template<typename DataBusType>
void SimManager<DataBusType>::display_sorted_app_info() {
  std::cout << "[Sim Manager] Sorted Application List\n\n";
  for (const std::shared_ptr<SimAppBase<DataBusType>>& app : app_list) {
      // Use RTTI to get the class name
      std::string class_name = typeid(*app).name();

      // Print class name, priority, and rate
      std::cout << "Class: " << class_name
                << " | Priority: " << app->priority
                << " | Time Step (s): " << app->app_dt_sec << '\n';
  }
  std::cout << "\n";
}

template<typename DataBusType>
void SimManager<DataBusType>::run() {
    sort_apps_by_priority();
    
    std::vector<std::future<void>> futures;

    for (std::size_t i = 0; i < num_mc_runs; i++) {
        SimSingleRunConfig config;
        config.start_time_sec        = start_time_sec;
        config.config_stop_time_sec  = config_stop_time_sec;  
        config.sim_rate_hz           = sim_rate_hz;
        config.base_file_name        = base_file_name;
        config.data_output_directory = data_output_directory;
        config.logging_rates         = logging_rates;
        config.current_mc_run        = i;
        config.random_seed           = init_seed + i;

        futures.push_back(std::async(std::launch::async, [this, config]() {
            run_single_mc(config);  // pass the per-run config
        }));

        if (futures.size() >= num_parallel_processors) {
            for (std::size_t j = 0; j < futures.size(); j++) {
                futures[j].get();
            }
            futures.clear();
        }
    }

    for (std::size_t j = 0; j < futures.size(); j++) {
        futures[j].get();
    }

    manager_teardown();
}

template<typename DataBusType>
void SimManager<DataBusType>::run_single_mc(const SimSingleRunConfig& config) {
    std::vector<std::shared_ptr<SimAppBase<DataBusType>>> copy_of_apps;
    for (std::size_t i = 0; i < app_list.size(); ++i) {
        const std::shared_ptr<SimAppBase<DataBusType>>& app = app_list[i];
        copy_of_apps.push_back(app->clone());
    }

    std::vector<std::shared_ptr<LoggingAppBase<DataBusType>>> copy_of_loggers;
    for (std::size_t i = 0; i < logging_apps.size(); ++i) {
        const std::shared_ptr<LoggingAppBase<DataBusType>>& log_app = logging_apps[i];
        copy_of_loggers.push_back(log_app->clone());
    }

    DataBusType bus_copy = data_bus;

    SimSingleRun<DataBusType> sim_run(copy_of_apps, copy_of_loggers, bus_copy, config);
    sim_run.run();
}

template<typename DataBusType>
void SimManager<DataBusType>::manager_teardown() {
    std::cout << "\n[Sim Manager] ALL RUNS FINISHED!!!\n";
}
