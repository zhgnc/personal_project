#include "simulation.hpp"

template<typename DataBusType>
Simulation<DataBusType>::Simulation(const std::string &path_to_sim_config, DataBusType& bus) : data_bus(bus) {
  YAML::Node config_data = load_yaml_file(path_to_sim_config);

  start_time_sec        = get_yaml_key<double>(config_data, "sim_start_time_sec");
  config_stop_time_sec  = get_yaml_key<double>(config_data, "sim_stop_time_sec");
  sim_rate_hz           = get_yaml_key<double>(config_data, "simulation_rate_hz");
  num_mc_runs           = get_yaml_key<std::size_t>(config_data, "number_of_monte_carlo_runs");
  init_seed             = get_yaml_key<uint64_t> (config_data, "initial_random_seed");
  print_hdf5_file_tree  = get_yaml_key<bool>(config_data, "print_hdf5_file_format");
  print_file_attributes = get_yaml_key<bool>(config_data, "print_hdf5_attributes_in_file_format");

  data_output_directory   = get_yaml_key<std::string>(config_data, "logging_file_save_directory");
  base_file_name          = get_yaml_key<std::string>(config_data, "logging_filename_prefix");
  logging_rates.rate_A_hz = get_yaml_key<double>(config_data, "logging_rate_A_hz");
  logging_rates.rate_B_hz = get_yaml_key<double>(config_data, "logging_rate_B_hz");
  logging_rates.rate_C_hz = get_yaml_key<double>(config_data, "logging_rate_C_hz");
  logging_rates.rate_D_hz = get_yaml_key<double>(config_data, "logging_rate_D_hz");
  logging_rates.rate_E_hz = get_yaml_key<double>(config_data, "logging_rate_E_hz");

  config_stop_time_usec = static_cast<uint64_t>(sec2usec * config_stop_time_sec);
  sim_dt_sec            = 1.0 / sim_rate_hz;
  sim_dt_usec           = static_cast<uint64_t>(sec2usec * (1.0 / sim_rate_hz)); 
  current_seed          = init_seed;
  app_count             = 0;
  logging_app_count     = 0;
  accessible_sim_data   = SimControl::AccessibleSimData{current_sim_time_sec, sim_dt_sec, sim_rate_hz, sim_step_count};
};



// This function accepts an object derived from `SimAppBase<DataBusType>` and 
// adds it to the `app_list`. A template AppType is used to preserve the virtual 
// functions that the `new_app` overwrote because if the input type was 
// `SimAppBase<DataBusType>` then the `new_app` specific behavior would be lost.
template<typename DataBusType>
template<typename AppType>
void Simulation<DataBusType>::add_app(AppType&& new_app) {
  static_assert(std::is_base_of_v<SimAppBase<DataBusType>, std::decay_t<AppType>>,
                "AppType must derive from SimAppBase<DataBusType>");
    
  if (app_count >= SimConfig::max_app_number) {
    throw std::runtime_error("[Simulation.tpp] Number of apps exceeded value of `max_app_number` in sim_config.hpp which is "
                             + std::to_string(SimConfig::max_app_number));
  }

  // The `make_shared` function uses `std::decay_t<AppType>` to ensure that the 
  // shared_ptr stores the actual derived app rather than base type. Then 
  // `std::forward<AppType>(app)` is used to allow flexibility of passing 
  // `new_app` as an lvalue or rvalue. Or simply `new_app` is added to the `app_list`
  app_list[app_count] = std::make_shared<std::decay_t<AppType>>(std::forward<AppType>(new_app));
  app_count = app_count + 1;
}

// See comments for `add_app()` to understand why templates, `decay_t`, and `forward`
template<typename DataBusType>
template<typename LoggingAppType>
void Simulation<DataBusType>::add_logging_app(LoggingAppType&& new_logging_app) {
  static_assert(std::is_base_of_v<LoggingAppBase<DataBusType>, std::decay_t<LoggingAppType>>,
                "LoggingAppType must derive from LoggingAppBase<DataBusType>");

  if (logging_app_count >= SimConfig::max_logging_app_number) {
    throw std::runtime_error("[Simulation.tpp] Number of logging apps exceeded value of `max_logging_app_number` in sim_config.hpp which is"
                             + std::to_string(SimConfig::max_logging_app_number));
  }

  logging_apps[logging_app_count] = std::make_shared<std::decay_t<LoggingAppType>>(std::forward<LoggingAppType>(new_logging_app));
  logging_app_count = logging_app_count + 1;
}

template<typename DataBusType>
void Simulation<DataBusType>::end_sim(const StopType& type, const StopReason& reason, const std::string& message) {
    if (stop_type != StopType::NoStop) { 
      return; 
    }

    stop_type            = type;
    stop_reason          = reason;
    stop_message         = message;
    actual_stop_time_sec = current_sim_time_sec;
    
    print_stop_diagnostics(type, reason, message);
}


template<typename DataBusType>
void Simulation<DataBusType>::print_stop_diagnostics(const StopType& type, const StopReason& reason, const std::string& message) {
  std::string type_string   = stop_type_to_string(type);
  int type_int              = static_cast<int>(type);
  std::string reason_string = stop_reason_to_string(reason);
  int reason_int            = static_cast<int>(reason);

  std::cout << "               | - Sim stop commanded\n"; 
  std::cout << "                   |- Type (enum): "   << type_string   << " || " << type_int   << std::endl;
  std::cout << "                   |- Reason (enum): " << reason_string << " || " << reason_int << std::endl;
  std::cout << "                   |- Message: " << message << std::endl;
}

template<typename DataBusType>
void Simulation<DataBusType>::update_accessible_sim_data() {
  accessible_sim_data = SimControl::AccessibleSimData{current_sim_time_sec, sim_dt_sec, sim_rate_hz, sim_step_count};
}

template<typename DataBusType>
uint64_t Simulation<DataBusType>::get_next_seed() {
  return current_seed++; // Returns then increments seed
}

template<typename DataBusType>
void Simulation<DataBusType>::run() {
  sort_apps_by_priority();
  display_sorted_app_info();
  initialize_apps();

  // For loop configured so that run_num starts at 1
  for (std::size_t run_num = 1; run_num < num_mc_runs + 1; run_num++) {
    run_setup(run_num);

    while (current_sim_time_usec <= config_stop_time_usec && stop_type == StopType::NoStop) {
      run_step();
    }

    run_teardown();
  }
}

template<typename DataBusType>
void Simulation<DataBusType>::sort_apps_by_priority() {
  std::cout << "[Simulation] Configuring Simulation\n";
  std::sort(app_list.begin(), app_list.begin() + app_count, Simulation::compare_by_priority);
};

template<typename DataBusType>
bool Simulation<DataBusType>::compare_by_priority(const std::shared_ptr<SimAppBase<DataBusType>> &app_A,
                                     const std::shared_ptr<SimAppBase<DataBusType>> &app_B) {
  // `<` sorts the apps to execute in ascending order (lower numbers run earlier) 
  return app_A->priority < app_B->priority;
};

template<typename DataBusType>
void Simulation<DataBusType>::display_sorted_app_info() {
  std::cout << "[Simulation] Sorted Application List\n";

  for (std::size_t i = 0; i < app_count; i++) {
    std::cout << "App Name: " << app_list[i]->name
              << " | Priority: " << app_list[i]->priority
              << " | Time Step (s): " << app_list[i]->app_dt_sec << '\n';
  }

  std::cout << "\n";
}

template<typename DataBusType>
void Simulation<DataBusType>::initialize_apps() {std::cout << "[Simulation] Initializing Apps\n";
  SimControl sim_ctrl = make_sim_control();
  
  for (std::size_t i = 0; i < app_count; i++) {
    app_list[i]->initialize(sim_ctrl);
  }

  std::cout << "[Simulation] Apps Initialized\n\n";
}

template<typename DataBusType>
void Simulation<DataBusType>::run_setup(std::size_t run_num) {
  std::cout << "[Simulation] Starting Simulation Run #" << run_num << std::endl;
  current_mc_run = run_num;

  initialize_pre_run_data();

  // `logger` must be reset per-run to avoid accumulation of meta data in hdf5 
  // class or else simulation execution will slow down during very long MC tests
  std::string new_file_name = std::format("{}_RUN_{:05}.hdf5", base_file_name, current_mc_run);
  std::string full_path     = data_output_directory + "/" + new_file_name;
  logger                    = std::make_unique<Logger>(full_path);

  LoggerFacade logger_facade(*logger);

  for (std::size_t i = 0; i < logging_app_count; i++) {
    logging_apps[i]->configure_hdf5_logging(logger_facade, data_bus, logging_rates);
  }

  sim_data_logger = std::make_unique<SimDataLogger>(*logger);
  sim_data_logger->configure_file_with_sim_data(current_sim_time_sec, current_sim_time_usec, sim_rate_hz, sim_step_count);

  computer_start_time = std::chrono::high_resolution_clock::now();
}

template<typename DataBusType>
void Simulation<DataBusType>::initialize_pre_run_data() {
  current_sim_time_usec = static_cast<uint64_t>(sec2usec * start_time_sec);
  current_sim_time_sec  = start_time_sec;
  sim_step_count        = 0;
  stop_type             = StopType::NoStop;
  stop_reason           = StopReason::ReachedConfiguredStopTime;
  stop_message          = "None Provided";
  actual_stop_time_sec  = config_stop_time_sec;
  accessible_sim_data   = SimControl::AccessibleSimData{current_sim_time_sec, sim_dt_sec, sim_rate_hz, sim_step_count};
}

template<typename DataBusType>
void Simulation<DataBusType>::run_step() {
  update_accessible_sim_data();
  SimControl sim_ctrl = make_sim_control();
  
  for (std::size_t i = 0; i < app_count; i++) {
    app_list[i]->check_step(current_sim_time_usec, data_bus, sim_ctrl);

    if (stop_type == StopType::AfterApp) {
      return;
    }
  }

  logger->log_data(current_sim_time_usec);
  
  current_sim_time_usec += sim_dt_usec;
  current_sim_time_sec   = current_sim_time_usec / sec2usec;
  sim_step_count         = sim_step_count + 1;
}

template<typename DataBusType>
void Simulation<DataBusType>::run_teardown() {
  update_accessible_sim_data();
  SimControl sim_ctrl = make_sim_control();

  for (std::size_t i = 0; i < app_count; i++) {
    app_list[i]->teardown(data_bus, sim_ctrl);
  }; 

  computer_stop_time       = std::chrono::high_resolution_clock::now();
  computer_elapsed_seconds = computer_stop_time - computer_start_time;
  sim_to_real_time_ratio   = actual_stop_time_sec / computer_elapsed_seconds.count();
  std::cout << "[Simulation] Run #" << current_mc_run << " ended after " << computer_elapsed_seconds.count() << "seconds (x" << sim_to_real_time_ratio << "faster than real time)\n";
  
  log_run_meta_data();

  // If this is the last run I want to print the file tree for reference if enabled
  // which must be done before `.reset()` is called below
  if (current_mc_run >= num_mc_runs && print_hdf5_file_tree == true) {
    sim_teardown();
  }
  
  sim_data_logger.reset(); // `.reset()` calls deconstructors
  logger.reset();
}

template<typename DataBusType>
void Simulation<DataBusType>::log_run_meta_data() {
  meta_data.start_time_sec           = start_time_sec; 
  meta_data.config_stop_time_sec     = config_stop_time_sec;
  meta_data.actual_stop_time_sec     = actual_stop_time_sec; 
  meta_data.stop_type                = stop_type;
  meta_data.stop_reason              = stop_reason;
  meta_data.stop_message             = stop_message;
  meta_data.sim_rate_hz              = sim_rate_hz; 
  meta_data.num_total_mc_runs        = num_mc_runs; 
  meta_data.current_mc_run           = current_mc_run; 
  meta_data.initial_random_seed      = init_seed;
  meta_data.computer_start_time      = computer_start_time; 
  meta_data.computer_stop_time       = computer_stop_time; 
  meta_data.computer_elapsed_seconds = computer_elapsed_seconds; 
  meta_data.sim_to_real_time_ratio   = sim_to_real_time_ratio;
  meta_data.app_count                = app_count;
  meta_data.logging_app_count        = logging_app_count;  
  
  sim_data_logger->log_sim_meta_data(meta_data);
}

template<typename DataBusType>
void Simulation<DataBusType>::sim_teardown() {
  std::cout << "\n[Simulation] ALL RUNS FINISHED!!!\n\n";
    
  std::cout << "[Simulation] HDF5 output files will have the following data\n";
  logger->print_file_tree(print_file_attributes);
}

template<typename DataBusType>
SimControl Simulation<DataBusType>::make_sim_control() {
  return SimControl(accessible_sim_data,
                    [this](StopType t, StopReason r, const std::string& m) { end_sim(t, r, m); },
                    [this]() { return get_next_seed(); });
}