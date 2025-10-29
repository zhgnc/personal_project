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
#include "sim_framework/data_bus.hpp"
#include "sim_data_logger/sim_logging_manager.hpp"

#include "../external/hdf5/include/H5Cpp.h"

#include "../examples/attitude_filter/apps/gyro_sim_app.hpp"
#include "../examples/attitude_filter/apps/logging_sim_app.hpp"

int main() {
  // const std::string &sim_config_path      = "examples/attitude_filter/config_files/simulation_config.yaml";
  // const std::string &logger_config_path   = "examples/attitude_filter/config_files/logging_config.yaml";
  // const std::string &priority_config_path = "examples/attitude_filter/config_files/app_priorities.yaml";
  // const std::string &gyro_config_path     = "examples/attitude_filter/config_files/gyro_config.yaml";

  // YAML::Node app_priority_data  = YAML::LoadFile(priority_config_path);
  // YAML::Node sim_config_data    = YAML::LoadFile(sim_config_path);
  // YAML::Node logger_config_data = YAML::LoadFile(logger_config_path);
  // YAML::Node gyro_config_data   = YAML::LoadFile(gyro_config_path);

  // DataBus data_bus;
  // Simulation sim(sim_config_path, data_bus);

  // double gyro_rate  = gyro_config_data["rate_hz"].as<double>();
  // int gyro_priority = app_priority_data["gyro_priority"].as<int>();
  // std::shared_ptr<SimAppBase> gyro_app = std::make_shared<GyroSimApp>(gyro_rate, gyro_priority, gyro_config_path);
  // sim.add_app(gyro_app);

  // std::shared_ptr<LoggingSimApp> logging_app = std::make_shared<LoggingSimApp>(logger_config_path);
  // sim.add_logger(logging_app);

  // sim.run();

  const std::string& logger_config_path = "examples/attitude_filter/config_files/logging_config.yaml";
  const std::string& test_file_path     = "C:/git/personal_project/src/test_zach_2.hdf5";
  SimLoggingManager logger(logger_config_path, test_file_path);

  logger.create_file();
  logger.add_group("group_1/group_3");
  logger.add_group("group_1/group_4");
  logger.add_group("group_2");
  logger.print_file_tree();

  
  double test_double    = 10.0;
  matrix<double,2,2> M1 = {1.0, 2.0, 3.0, 4.0};
  vector<float,3> V1    = {1.0, 2.0, 3.0};
  
  int record_freq_hz     = 1;

  logger.add_dataset<double>("Test_Data_Point_1", "group_1", std::make_shared<double>(test_double), record_freq_hz);
  logger.add_dataset<matrix<double,2,2>>("Test_Data_Point_2", "group_1/group_3", std::make_shared<matrix<double,2,2>>(M1), record_freq_hz);
  logger.add_dataset<vector<float,3>>("Test_Data_Point_3", "group_1/group_4", std::make_shared<vector<float,3>>(V1), record_freq_hz);

  logger.log();





  // HDF5Logger logger("zach_test.hdf5", "C:/git/personal_project/src/");



  // HDF5Logger logger("zach_test.hdf5", "C:/git/personal_project/src/");
  // logger.create_file();

  // logger.add_group("group_root/group_a/group_b");
  // logger.add_group("group_root/group_c/group_d");
  // logger.add_group("group_root/group_c/group_e");
  // logger.add_group("root_group_2/group_f/group_g");

  // logger.add_dataset<double>("test_data_name", {2, 3, 4}, "group_root");
  // logger.add_sim_dataset<double>("sim_data_test", {2, 3, 4}, "root_group_2");
  // logger.add_sim_dataset<double>("sim_data_test", {2, 3, 4}, "group_root/group_a/group_b");
  // logger.print_file_tree();

  // double data_to_log = 10.0;
  // std::unique_ptr<double[]> data_buffer_in(new double[1]);
  // data_buffer_in.get()[0] = data_to_log;

  // logger.write_data<double>("group_root/test_data_name", data_buffer_in.get(), {0, 0, 0}, {1, 1, 1});

  // std::unique_ptr<double[]> data_buffer_out(new double[24]);

  // logger.read_data<double>("group_root/test_data_name", data_buffer_out.get(), {0, 0, 0}, {2, 3, 4});

  // for (size_t i = 0; i < 24; i++) {
  //     std::cout << data_buffer_out[i] << " ";
  // }
  // std::cout << std::endl;

  // // logger.close_file();


  // // APPEND DATA TEST
  // // logger.open_file();

  // double sim_step_data[2 * 3 * 4] = {
  //   0.0, 0.1, 0.2, 0.3,
  //   1.0, 1.1, 1.2, 1.3,
  //   2.0, 2.1, 2.2, 2.3,

  //   3.0, 3.1, 3.2, 3.3,
  //   4.0, 4.1, 4.2, 4.3,
  //   5.0, 5.1, 5.2, 5.3
  // };



  // logger.append_data<double>("root_group_2/sim_data_test", {1, 2, 3, 4}, sim_step_data);
  // logger.read_data<double>("root_group_2/sim_data_test", data_buffer_out.get(), {0, 0, 0, 0}, {1, 2, 3, 4});


  // for (size_t i = 0; i < 24; i++) {
  //     std::cout << data_buffer_out[i] << " ";
  // }
  // std::cout << std::endl;

  // logger.close_file();

  // // // for (std::size_t i = 0; i < 5; i++ ) {

  // // // }

  return 0;
};
