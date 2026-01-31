#ifndef RUN_ATTITUDE_FILTER_HPP
#define RUN_ATTITUDE_FILTER_HPP

#include <array>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include "math/math.hpp"

#include "sim_framework/sim_includes.hpp"
#include "utilities/yaml_utilities.hpp"

#include "sim_files/gyro_sim_app.hpp"
#include "sim_files/logging_sim_app.hpp"
#include "sim_files/data_bus.hpp"

int run_attitude_filter() {
  std::string sim_and_logger_config_path = "projects/attitude_filter/config_files/sim_and_logging_config.yaml";
  std::string priority_config_path       = "projects/attitude_filter/config_files/app_priorities.yaml";
  std::string gyro_config_path           = "projects/attitude_filter/config_files/gyro_config.yaml";

  YAML::Node app_priority_data = load_yaml_file(priority_config_path);
  YAML::Node sim_config_data   = load_yaml_file(sim_and_logger_config_path);
  YAML::Node gyro_config_data  = load_yaml_file(gyro_config_path);

  DataBus data_bus;
  Simulation sim(sim_and_logger_config_path, data_bus);

  std::string gyro_app_name = "Gyro App";
  double gyro_rate  = get_yaml_value<double>(gyro_config_data, "rate_hz");
  int gyro_priority = get_yaml_value<int>(app_priority_data, "gyro_priority");
  GyroSimApp gyro_app(gyro_app_name, gyro_rate, gyro_priority, gyro_config_path);
  sim.add_app(gyro_app);

  LoggingSimApp logger;
  sim.add_logging_app(logger);

  sim.run();

  return 0;
};

#endif