// Copyright 2025 Zachary Hart

#include <array>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include "math_utilities/math.hpp"
#include "yaml-cpp/yaml.h"

#include "sim_framework/sim_app.hpp"
#include "sim_framework/simulation.hpp"
#include "sim_framework/data_bus.hpp"
#include "logger/hdf5_logger.hpp"

#include "../external/hdf5/include/H5Cpp.h"

#include "../examples/attitude_filter/sim_apps/gyro_sim_interface.hpp"

int main() {
  // const std::string &sim_config_path      = "examples/attitude_filter/config_files/simulation_config.yaml";
  // const std::string &priority_config_path = "examples/attitude_filter/config_files/app_priorities.yaml";
  // const std::string &gyro_config_path     = "examples/attitude_filter/config_files/gyro_config.yaml";

  // YAML::Node app_priority_data = YAML::LoadFile(priority_config_path);
  // YAML::Node gyro_config_data = YAML::LoadFile(gyro_config_path);

  // DataBus data_bus;
  // Simulation sim(sim_config_path, data_bus);

  // double gyro_rate = gyro_config_data["rate_hz"].as<double>();
  // int gyro_priority = app_priority_data["gyro_priority"].as<int>();
  // std::shared_ptr<GyroSimInterface> gyro_interface = std::make_shared<GyroSimInterface>();
  // std::shared_ptr<SimApp> gyro_app = std::make_shared<SimApp>(gyro_interface, gyro_rate, gyro_priority, gyro_config_path);
  // sim.add_app(gyro_app);

  // sim.run();

  HDF5Logger logger("zach_test.hdf5", "C:/git/personal_project/src/");
  logger.create_file();

  logger.add_group("group_root/group_a/group_b");
  logger.add_group("group_root/group_c/group_d");
  logger.add_group("group_root/group_c/group_e");
  logger.add_group("root_group_2/group_f/group_g");

  std::array<hsize_t, 3> data_dimensions = {2, 3, 4}; // Tells hdf5 logger this data set is 2x3x4
  logger.add_dataset<double>("test_data_name", data_dimensions, "group_root");
  logger.add_sim_dataset<double>("sim_data_test", data_dimensions, "root_group_2");
  logger.add_sim_dataset<double>("sim_data_test", data_dimensions, "group_root/group_a/group_b");
  logger.print_file_tree();
  logger.close_file();

  // for (std::size_t i = 0; i < 5; i++ ) {

  // }

  return 0;
};
