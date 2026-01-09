#include "simulation.hpp"

#include "yaml-cpp/yaml.h"
#include <chrono>
#include <iostream>

template<typename DataBusType>
Simulation<DataBusType>::Simulation(const std::string &path_to_sim_config, DataBusType& bus) 
  : data_bus(bus)
{
  YAML::Node config_data = YAML::LoadFile(path_to_sim_config);

  start_time_sec   = config_data["sim_start_time_sec"].as<double>();
  stop_time_sec    = config_data["sim_stop_time_sec"].as<double>();
  sim_rate_hz      = config_data["simulation_rate_hz"].as<double>();
  num_mc_runs      = config_data["number_of_monte_carlo_runs"].as<std::size_t>();

  sim_dt_usec           = static_cast<uint32_t>(sec2usec * (1.0 / sim_rate_hz));
  current_sim_time_usec = static_cast<uint32_t>(sec2usec * start_time_sec);
  current_sim_time_sec  = start_time_sec;
  stop_time_usec        = static_cast<uint32_t>(sec2usec * stop_time_sec);
};

template<typename DataBusType>
void Simulation<DataBusType>::add_app(std::shared_ptr<SimAppBase<DataBusType>> new_app) {
  app_list.push_back(new_app);
};

template<typename DataBusType>
void Simulation<DataBusType>::add_logger(std::shared_ptr<LoggingAppBase<DataBusType>> logger) {
  logging_app     = logger;
  sim_data_logger = std::make_unique<SimDataLogger>(logging_app->logger);

  // Without creating a file print_file_tree cannot output the file tree. Using 
  // file number 1 because it will be overwritten during first simulation run
  std::cout << "\n[Simulation] Output HDF5 file will have the following structure" << std::endl;
  logging_app->create_new_file(1);
  logging_app->logger.print_file_tree();
}

template<typename DataBusType>
void Simulation<DataBusType>::sort_apps_by_priority() {
  std::sort(app_list.begin(), app_list.end(), Simulation::compare_by_priority);
};

template<typename DataBusType>
bool Simulation<DataBusType>::compare_by_priority(const std::shared_ptr<SimAppBase<DataBusType>> &app_A,
                                     const std::shared_ptr<SimAppBase<DataBusType>> &app_B) {
  // ">" gives lower numbers a higher priority
  return app_A->priority < app_B->priority;
};

template<typename DataBusType>
void Simulation<DataBusType>::initialize_apps() {std::cout << "[Simulation] Initializing Apps\n";
  for (std::shared_ptr<SimAppBase<DataBusType>> &app : app_list) {
    app->initialize(data_bus);
  }

  logging_app->set_data_source(data_bus);
  std::cout << "[Simulation] Apps Initialized\n\n";
}

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
void Simulation<DataBusType>::run() {
  std::cout << "[Simulation] Configuring Simulation\n";

  sort_apps_by_priority();
  // display_sorted_app_info();
  initialize_apps();

  // run_num starts at 1 so that when num_mc_runs = 1 the current_mc_run != 0 and = 1 in the meta data
  for (std::size_t run_num = 1; run_num < num_mc_runs+1; run_num++) {
    std::cout << "[Simulation] Starting Simulation Run #" << run_num << std::endl;
    current_mc_run = run_num;

    logging_app->create_new_file(run_num);

    current_sim_time_sec  = start_time_sec;
    current_sim_time_usec = current_sim_time_sec * sec2usec;

    computer_start_time   = std::chrono::high_resolution_clock::now();

    while (current_sim_time_usec <= stop_time_usec) {

      for (std::shared_ptr<SimAppBase<DataBusType>> &app : app_list) {
        app->check_step(current_sim_time_usec);
      }

      logging_app->log_data(current_sim_time_usec);

      current_sim_time_usec += sim_dt_usec;
      current_sim_time_sec   = current_sim_time_usec / sec2usec;
    }
    
    Simulation::run_teardown();
  }
}

template<typename DataBusType>
void Simulation<DataBusType>::run_teardown() {  
    computer_stop_time       = std::chrono::high_resolution_clock::now();
    computer_elapsed_seconds = computer_stop_time - computer_start_time;
    sim_to_real_time_ratio   = stop_time_sec / computer_elapsed_seconds.count();
    
    SimMetaData meta_data{
      start_time_sec,
      stop_time_sec,
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

    std::cout << "[Simulation] Run #" << current_mc_run << " ended after " << computer_elapsed_seconds.count() << "seconds (x" << sim_to_real_time_ratio << "faster than real time)\n";
};