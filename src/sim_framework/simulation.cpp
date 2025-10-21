#include "simulation.hpp"

#include "yaml-cpp/yaml.h"
#include <chrono>
#include <iostream>

Simulation::Simulation(const std::string &path_to_sim_config, DataBus& bus) {
  YAML::Node config_data = YAML::LoadFile(path_to_sim_config);

  start_time_sec   = config_data["sim_start_time_sec"].as<double>();
  stop_time_sec    = config_data["sim_stop_time_sec"].as<double>();
  sim_rate_hz      = config_data["simulation_rate_hz"].as<double>();
  num_mc_runs      = config_data["number_of_monte_carlo_runs"].as<std::size_t>();

  sim_dt_usec           = static_cast<uint32_t>(sec2usec * (1.0 / sim_rate_hz));
  current_sim_time_usec = static_cast<uint32_t>(sec2usec * start_time_sec);
  current_sim_time_sec  = start_time_sec;
  stop_time_usec        = static_cast<uint32_t>(sec2usec * stop_time_sec);

  data_bus = bus;
};

void Simulation::add_app(std::shared_ptr<SimApp> new_app) {
  app_list.push_back(new_app);
};

void Simulation::add_logger(std::shared_ptr<LoggingAppBase> logger) {
  data_logger = logger;
}

void Simulation::sort_apps_by_priority() {
  std::sort(app_list.begin(), app_list.end(), Simulation::compare_by_priority);
};

bool Simulation::compare_by_priority(const std::shared_ptr<SimApp> &app_A,
                                     const std::shared_ptr<SimApp> &app_B) {
  // ">" sorts by ascending piority (lower number = higher priority)
  return app_A->priority > app_B->priority;
};

void Simulation::initialize_apps() {std::cout << "[Simulation] Initializing Apps\n";
  for (std::shared_ptr<SimApp> &app : app_list) {
    app->initialize(data_bus);
  }
  std::cout << "[Simulation] Apps Initialized\n\n";
}

void Simulation::run() {
  std::cout << "[Simulation] Starting Simulation\n";

  sort_apps_by_priority();
  initialize_apps();

  for (std::size_t run_num = 0; run_num < num_mc_runs; run_num++) {
    computer_start_time = std::chrono::high_resolution_clock::now();
    // std::cout << "[Simulation] Run #" << run_num << " started\n";

    data_logger->create_new_file(run_num);
    current_sim_time_usec = start_time_sec;
    current_sim_time_sec  = current_sim_time_usec / sec2usec;

    // std::cout << "[Simulation] Time (sec, usec): " << current_sim_time_sec << "     " << current_sim_time_usec << "\n";

    while (current_sim_time_usec <= stop_time_usec) {

      for (std::shared_ptr<SimApp> &app : app_list) {
        app->step(current_sim_time_usec);
      }

      data_logger->log_data(current_sim_time_usec);

      current_sim_time_usec += sim_dt_usec;
      current_sim_time_sec = current_sim_time_usec / sec2usec;

      // std::cout << "[Simulation] Time (sec, usec): " << current_sim_time_sec << "     " << current_sim_time_usec << "\n";
    }
    
    data_logger->logger.close_file();

    computer_stop_time = std::chrono::high_resolution_clock::now();
    computer_elapsed_seconds = computer_stop_time - computer_start_time;
    sim_to_real_time = stop_time_sec / computer_elapsed_seconds.count();
    std::cout << "[Simulation] Run #" << run_num << " ended after " << computer_elapsed_seconds.count() << "(x" << sim_to_real_time << " real time)\n";
  }
}
