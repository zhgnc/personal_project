#include "simulation.hpp"

template<typename DataBusType>
Simulation<DataBusType>::Simulation(const std::string &path_to_sim_config, DataBusType& bus) 
  : data_bus(bus)
{
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

  sim_dt_sec            = 1.0 / sim_rate_hz;
  sim_dt_usec           = static_cast<uint64_t>(sec2usec * (1.0 / sim_rate_hz));
  current_sim_time_usec = static_cast<uint64_t>(sec2usec * start_time_sec);
  current_sim_time_sec  = start_time_sec;
  config_stop_time_usec = static_cast<uint64_t>(sec2usec * config_stop_time_sec);
  sim_step_count        = 0; 
  current_seed          = init_seed;

  stop_sim_after_cycle  = false;
  stop_sim_now          = false;
  stop_type             = StopType::NoStop;
  stop_reason           = StopReason::ReachedConfiguredStopTime;
  stop_message          = "None Provided";
  actual_stop_time_sec  = config_stop_time_sec; 

  accessible_sim_data   = AccessibleSimData{current_sim_time_sec, sim_dt_sec, sim_rate_hz, sim_step_count};
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
    
  // The `push_back` function uses `std::decay_t<AppType>` to ensure that the 
  // shared_ptr stores the actual derived app rather than base type. Then 
  // `std::forward<AppType>(app)` is used to allow flexibility of passing 
  // `new_app` as an lvalue or rvalue. Simply `new_app` is added to the `app_list`
  app_list.push_back(std::make_shared<std::decay_t<AppType>>(std::forward<AppType>(new_app)));
}

// See comments for `add_app()` to understand why templates, `decay_t`, and `forward`
template<typename DataBusType>
template<typename LoggingAppType>
void Simulation<DataBusType>::add_logging_app(LoggingAppType&& new_logging_app) {
    static_assert(std::is_base_of_v<LoggingAppBase<DataBusType>, std::decay_t<LoggingAppType>>,
                  "LoggingAppType must derive from LoggingAppBase<DataBusType>");

    logging_apps.push_back(std::make_shared<std::decay_t<LoggingAppType>>(std::forward<LoggingAppType>(new_logging_app)));
}

template<typename DataBusType>
void Simulation<DataBusType>::end_sim_after_cycle(const StopReason& reason, const std::string& message) { 
  print_stop_diagnostics(StopType::AfterCycle, reason, message);
  bool first_request_to_end_sim = stop_sim_after_cycle == false && stop_sim_now == false;
  
  if (first_request_to_end_sim == true) {
    stop_sim_after_cycle = true;
    stop_type            = StopType::AfterCycle;
    stop_reason          = reason;
    stop_message         = message;
    actual_stop_time_sec = current_sim_time_sec;
  }
}

template<typename DataBusType>
void Simulation<DataBusType>::end_sim_after_app(const StopReason& reason, const std::string& message) {
  print_stop_diagnostics(StopType::AfterApp, reason, message);
  bool first_request_to_end_sim = stop_sim_after_cycle == false && stop_sim_now == false;

  if (first_request_to_end_sim == true) {
    stop_sim_now         = true;
    stop_type            = StopType::AfterApp;
    stop_reason          = reason;
    stop_message         = message;
    actual_stop_time_sec = current_sim_time_sec;
  }
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
bool Simulation<DataBusType>::stop_requested() const {
  return stop_sim_after_cycle == true || stop_sim_now == true;
}

template<typename DataBusType>
const SimulationControl::AccessibleSimData& Simulation<DataBusType>::public_sim_data() const {
  return accessible_sim_data;
}

template<typename DataBusType>
void Simulation<DataBusType>::update_accessible_sim_data() {
  accessible_sim_data = AccessibleSimData{current_sim_time_sec, sim_dt_sec, sim_rate_hz, sim_step_count};
}

template<typename DataBusType>
uint64_t Simulation<DataBusType>::get_next_seed() {
  return current_seed++; // Returns seed an increments
}

template<typename DataBusType>
void Simulation<DataBusType>::run() {
  sort_apps_by_priority();
  display_sorted_app_info();
  initialize_apps();

  // run_num starts at 1 so that when num_mc_runs = 1 the current_mc_run != 0 and = 1 in the metadata
  for (std::size_t run_num = 1; run_num < num_mc_runs+1; run_num++) {
    run_setup(run_num);

    while (current_sim_time_usec <= config_stop_time_usec && stop_requested() == false) {
      run_step();
    }

    run_teardown();
  }

  sim_teardown();
}

template<typename DataBusType>
void Simulation<DataBusType>::sort_apps_by_priority() {
  std::cout << "[Simulation] Configuring Simulation\n";
  std::sort(app_list.begin(), app_list.end(), Simulation::compare_by_priority);
};

template<typename DataBusType>
bool Simulation<DataBusType>::compare_by_priority(const std::shared_ptr<SimAppBase<DataBusType>> &app_A,
                                     const std::shared_ptr<SimAppBase<DataBusType>> &app_B) {
  // `<` sorts the apps to execute in ascending order (lower numbers run earlier) 
  return app_A->priority < app_B->priority;
};

template<typename DataBusType>
void Simulation<DataBusType>::display_sorted_app_info() {
  std::cout << "[Simulation] Sorted Application List\n\n";
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
void Simulation<DataBusType>::initialize_apps() {std::cout << "[Simulation] Initializing Apps\n";
  SimulationControl& sim_ctrl = *this;
  
  for (std::shared_ptr<SimAppBase<DataBusType>> &app : app_list) {
    app->initialize(sim_ctrl);
  }

  sim_data_logger = std::make_unique<SimDataLogger>(logger);
  std::cout << "[Simulation] Apps Initialized\n\n";
}

template<typename DataBusType>
void Simulation<DataBusType>::run_setup(std::size_t run_num) {
  // std::cout << "[Simulation] Starting Simulation Run #" << run_num << std::endl;
  current_mc_run = run_num;

  std::string new_file_name = std::format("{}_RUN_{:05}.hdf5", base_file_name, current_mc_run);
  std::string full_path     = data_output_directory + "/" + new_file_name;
  logger.create_file(full_path);

  for (std::shared_ptr<LoggingAppBase<DataBusType>>& app : logging_apps) {
    app->config_hdf5_with_app_data(logger, data_bus, logging_rates);
  }

  sim_data_logger->configure_file_with_sim_data(current_sim_time_sec, current_sim_time_usec, sim_rate_hz, sim_step_count);

  current_sim_time_sec  = start_time_sec;
  current_sim_time_usec = current_sim_time_sec * sec2usec;
  computer_start_time   = std::chrono::high_resolution_clock::now();

  update_accessible_sim_data();
}

template<typename DataBusType>
void Simulation<DataBusType>::run_step() {
  SimulationControl& sim_ctrl = *this;
  
  for (std::shared_ptr<SimAppBase<DataBusType>> &app : app_list) {
    app->check_step(current_sim_time_usec, data_bus, sim_ctrl);

    if (stop_sim_now == true) {
      return;
    }
  }

  logger.log_data(current_sim_time_usec);

  if (stop_sim_after_cycle == true) {
    return;
  }

  current_sim_time_usec += sim_dt_usec;
  current_sim_time_sec   = current_sim_time_usec / sec2usec;
  sim_step_count         = sim_step_count + 1;

  update_accessible_sim_data();
}

template<typename DataBusType>
void Simulation<DataBusType>::run_teardown() {  
  computer_stop_time       = std::chrono::high_resolution_clock::now();
  computer_elapsed_seconds = computer_stop_time - computer_start_time;
  sim_to_real_time_ratio   = actual_stop_time_sec / computer_elapsed_seconds.count();
  
  assign_meta_data();
  sim_data_logger->log_sim_meta_data(meta_data);
  logger.close_file();

  sim_step_count       = 0;
  stop_sim_now         = false;
  stop_sim_after_cycle = false;

  std::cout << "[Simulation] Run #" << current_mc_run << " ended after " << computer_elapsed_seconds.count() << "seconds (x" << sim_to_real_time_ratio << "faster than real time)\n";
}

template<typename DataBusType>
void Simulation<DataBusType>::assign_meta_data() {
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
}

template<typename DataBusType>
void Simulation<DataBusType>::sim_teardown() {  
  std::cout << "\n[Simulation] ALL RUNS FINISHED!!!\n\n";
  
  if (print_hdf5_file_tree == true) {
    std::cout << "[Simulation] HDF5 output files will have the following data\n";
    logger.print_file_tree(print_file_attributes);
  }
}