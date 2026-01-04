// Copyright 2025 Zachary Hart

#include <array>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include "math_utilities/math.hpp"
#include "yaml-cpp/yaml.h"

#include "sim_framework/sim_app_base.hpp"
#include "sim_framework/simulation.hpp"
#include "data_logging/logger.hpp"

#include "../external/hdf5/include/H5Cpp.h"

#include "../examples/attitude_filter/apps/gyro_sim_app.hpp"
#include "../examples/attitude_filter/apps/logging_sim_app.hpp"
#include "../examples/attitude_filter/apps/data_bus.hpp"

int main() {
  const std::string &sim_config_path      = "examples/attitude_filter/config_files/simulation_config.yaml";
  const std::string &logger_config_path   = "examples/attitude_filter/config_files/logging_config.yaml";
  const std::string &priority_config_path = "examples/attitude_filter/config_files/app_priorities.yaml";
  const std::string &gyro_config_path     = "examples/attitude_filter/config_files/gyro_config.yaml";

  YAML::Node app_priority_data  = YAML::LoadFile(priority_config_path);
  YAML::Node sim_config_data    = YAML::LoadFile(sim_config_path);
  YAML::Node logger_config_data = YAML::LoadFile(logger_config_path);
  YAML::Node gyro_config_data   = YAML::LoadFile(gyro_config_path);

  DataBus data_bus;
  Simulation sim(sim_config_path, data_bus);

  double gyro_rate  = gyro_config_data["rate_hz"].as<double>();
  int gyro_priority = app_priority_data["gyro_priority"].as<int>();
  std::shared_ptr<SimAppBase<DataBus>> gyro_app = std::make_shared<GyroSimApp>(gyro_rate, gyro_priority, gyro_config_path);
  sim.add_app(gyro_app);

  std::shared_ptr<LoggingSimApp> logging_app = std::make_shared<LoggingSimApp>(logger_config_path);
  sim.add_logger(logging_app);

  sim.run();

  return 0;
};
