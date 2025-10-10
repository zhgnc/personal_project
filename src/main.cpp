// Copyright 2025 Zachary Hart

#include <array>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include "math_utilities/math.hpp"
#include "yaml-cpp/yaml.h"

#include "sim_framework/simulation.hpp"
#include "sim_framework/sim_app.hpp"

#include "../examples/attitude_filter/sim_apps/gyro_sim_interface.hpp"

int main() {
  const std::string& sim_config_path      = "examples/attitude_filter/config_files/simulation_config.yaml";
  const std::string& priority_config_path = "examples/attitude_filter/config_files/app_priorities.yaml";
  const std::string& gyro_config_path     = "examples/attitude_filter/config_files/gyro_config.yaml";

  YAML::Node app_priority_data = YAML::LoadFile(priority_config_path);
  YAML::Node gyro_config_data  = YAML::LoadFile(gyro_config_path);

  
  Simulation sim(sim_config_path);
  

  double gyro_rate  = gyro_config_data["rate_hz"].as<double>();
  int gyro_priority = app_priority_data["gyro_priority"].as<int>();
  std::shared_ptr<GyroSimInterface> gyro_interface = std::make_shared<GyroSimInterface>();
  std::shared_ptr<SimApp> gyro_app = std::make_shared<SimApp>(gyro_interface, gyro_rate, gyro_priority, gyro_config_path);
  sim.add_app(gyro_app);
  
  
  sim.run();

  return 0;
};
