#include "simulation.hpp"

#include "yaml-cpp/yaml.h"
#include <chrono>
#include <iostream>

template<typename DataBusType>
Simulation<DataBusType>::Simulation(const std::string &path_to_sim_config, DataBusType& bus) 
  : data_bus(bus)
{
  YAML::Node config_data = YAML::LoadFile(path_to_sim_config);

  start_time_sec        = config_data["sim_start_time_sec"].as<double>();
  config_stop_time_sec  = config_data["sim_stop_time_sec"].as<double>();
  sim_rate_hz           = config_data["simulation_rate_hz"].as<double>();
  num_mc_runs           = config_data["number_of_monte_carlo_runs"].as<std::size_t>();
  print_hdf5_file_tree  = config_data["print_hdf5_file_format"].as<bool>();
  print_file_attributes = config_data["print_hdf5_attributes_in_file_format"].as<bool>();

  sim_dt_usec           = static_cast<uint64_t>(sec2usec * (1.0 / sim_rate_hz));
  current_sim_time_usec = static_cast<uint64_t>(sec2usec * start_time_sec);
  current_sim_time_sec  = start_time_sec;
  config_stop_time_usec = static_cast<uint64_t>(sec2usec * config_stop_time_sec);

  end_sim               = false;
  stop_reason           = StopReason::ReachedConfiguredStopTime;
  actual_stop_time_sec  = config_stop_time_sec; 
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
template<typename LoggerType>
void Simulation<DataBusType>::add_logger(LoggerType&& logger) {
  static_assert(std::is_base_of_v<LoggingAppBase<DataBusType>, std::decay_t<LoggerType>>,
                "Logger must derive from LoggingAppBase");

  logging_app = std::make_shared<std::decay_t<LoggerType>>(std::forward<LoggerType>(logger));

  // Constructing with the logger in `logging_app` so one logger logs both sim and user data
  sim_data_logger = std::make_unique<SimDataLogger>(logging_app->logger);
}

template<typename DataBusType>
void Simulation<DataBusType>::stop_sim(StopReason reason, const std::string& message) {
  // Conditional only saves the reason and message from the first request 
  if (end_sim == false) {
      end_sim              = true;
      stop_reason          = reason;
      stop_message         = message;
      actual_stop_time_sec = current_sim_time_sec;

      std::cout << "[             | - Sim stop commanded\n"; 
      std::cout << "                   |- Reason (enum): " << stop_reason_to_string(reason) << " || " << static_cast<int>(reason) << std::endl;
      std::cout << "                   |- Message: " << message << std::endl;
  }
}

template<typename DataBusType>
void Simulation<DataBusType>::run() {
  sort_apps_by_priority();
  display_sorted_app_info();
  initialize_apps();

  // run_num starts at 1 so that when num_mc_runs = 1 the current_mc_run != 0 and = 1 in the metadata
  for (std::size_t run_num = 1; run_num < num_mc_runs+1; run_num++) {
    run_setup(run_num);

    while (current_sim_time_usec <= config_stop_time_usec && end_sim == false) {
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
  // ">" gives lower numbers a higher priority
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
  for (std::shared_ptr<SimAppBase<DataBusType>> &app : app_list) {
    app->initialize();
  }

  logging_app->set_data_source(data_bus);
  std::cout << "[Simulation] Apps Initialized\n\n";
}

template<typename DataBusType>
void Simulation<DataBusType>::run_setup(std::size_t run_num) {
  std::cout << "[Simulation] Starting Simulation Run #" << run_num << std::endl;
  current_mc_run = run_num;

  logging_app->create_new_file(run_num);
  sim_data_logger->configure_file_with_sim_data(current_sim_time_sec, current_sim_time_usec, sim_rate_hz);

  current_sim_time_sec  = start_time_sec;
  current_sim_time_usec = current_sim_time_sec * sec2usec;

  computer_start_time   = std::chrono::high_resolution_clock::now();
}

template<typename DataBusType>
void Simulation<DataBusType>::run_step() {
  SimulationControl& sim_ctrl = *this;
  
  for (std::shared_ptr<SimAppBase<DataBusType>> &app : app_list) {
    app->check_step(current_sim_time_usec, data_bus, sim_ctrl);
  }

  // The logging_app logs user data (via the logging_app_base) and simulation data (via 
  // the sim_data_logger) because the logging_app & sim_data_logger class members share 
  // the same logger. The sim_data_logger object is constructed with the same logger in 
  // the `add_logger` function above.  
  logging_app->log_data(current_sim_time_usec);

  if (end_sim == true) {
    return;
  }

  current_sim_time_usec += sim_dt_usec;
  current_sim_time_sec   = current_sim_time_usec / sec2usec;
}

template<typename DataBusType>
void Simulation<DataBusType>::run_teardown() {  
    computer_stop_time       = std::chrono::high_resolution_clock::now();
    computer_elapsed_seconds = computer_stop_time - computer_start_time;
    sim_to_real_time_ratio   = actual_stop_time_sec / computer_elapsed_seconds.count();
    
    SimMetaData meta_data{
      start_time_sec,
      config_stop_time_sec,
      actual_stop_time_sec,
      stop_reason,
      stop_message,
      sim_rate_hz,
      num_mc_runs,
      current_mc_run,
      computer_start_time,
      computer_stop_time,
      computer_elapsed_seconds,
      sim_to_real_time_ratio
    };

    sim_data_logger->log_sim_meta_data(meta_data);
    logging_app->close_file();

    end_sim = false;

    std::cout << "[Simulation] Run #" << current_mc_run << " ended after " << computer_elapsed_seconds.count() << "seconds (x" << sim_to_real_time_ratio << "faster than real time)\n";
}

template<typename DataBusType>
void Simulation<DataBusType>::sim_teardown() {  
  std::cout << "\n[Simulation] ALL RUNS FINISHED!!!\n\n";
  
  if (print_hdf5_file_tree == true) {
    std::cout << "[Simulation] HDF5 output files will have the following data\n";
    logging_app->logger.print_file_tree(print_file_attributes);
  }
}