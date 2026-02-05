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
#include "sim_files/logging_sim_app.hpp"
#include "sim_files/data_bus.hpp"

#include "sim_files/fake_dynamics_sim_app.hpp"
#include "sim_files/gyro_sim_app.hpp"
#include "sim_files/star_tracker_sim_app.hpp"
#include "sim_files/attitude_filter_sim_app.hpp"

int run_attitude_filter() {
  std::string sim_and_logger_config_path    = "projects/attitude_filter/config_files/sim_and_logging_config.yaml";
  std::string priority_and_rate_config_path = "projects/attitude_filter/config_files/app_priorities_and_rates.yaml";
  std::string fake_dynamics_config_path     = "projects/attitude_filter/config_files/fake_dynamics_config.yaml";
  std::string gyro_config_path              = "projects/attitude_filter/config_files/gyro_config.yaml";
  std::string star_tracker_config_path      = "projects/attitude_filter/config_files/star_tracker_config.yaml";
  std::string attitude_filter_config_path   = "projects/attitude_filter/config_files/attitude_filter_config.yaml";


  YAML::Node app_priority_and_rate_data = load_yaml_file(priority_and_rate_config_path);

  DataBus data_bus;
  Simulation sim(sim_and_logger_config_path, data_bus);

  std::string dynamics_app_name = "Fake Dynamics App";
  double dynamics_rate  = get_yaml_value<double>(app_priority_and_rate_data, "dynamics_rate_hz");
  int dynamics_priority = get_yaml_value<int>(app_priority_and_rate_data, "dynamics_priority");
  FakeDynamicsSimApp dynamics_app(dynamics_app_name, dynamics_rate, dynamics_priority, fake_dynamics_config_path);
  sim.add_app(dynamics_app);

  std::string gyro_app_name = "Gyro App";
  double gyro_rate  = get_yaml_value<double>(app_priority_and_rate_data, "gyro_rate_hz");
  int gyro_priority = get_yaml_value<int>(app_priority_and_rate_data, "gyro_priority");
  GyroSimApp gyro_app(gyro_app_name, gyro_rate, gyro_priority, gyro_config_path);
  sim.add_app(gyro_app);

  std::string star_tracker_app_name = "Star Tracker App";
  double star_tracker_rate  = get_yaml_value<double>(app_priority_and_rate_data, "star_tracker_rate_hz");
  int star_tracker_priority = get_yaml_value<int>(app_priority_and_rate_data, "star_tracker_priority");
  StarTrackerSimApp star_tracker_app(star_tracker_app_name, star_tracker_rate, star_tracker_priority, star_tracker_config_path);
  sim.add_app(star_tracker_app);

  std::string attitude_filter_app_name = "Attitude Filter App";
  double attitude_filter_rate  = get_yaml_value<double>(app_priority_and_rate_data, "attitude_filter_rate_hz");
  int attitude_filter_priority = get_yaml_value<int>(app_priority_and_rate_data, "attitude_filter_priority");
  AttitudeFilterSimApp attitude_filter_app(attitude_filter_app_name, attitude_filter_rate, attitude_filter_priority, attitude_filter_config_path);
  sim.add_app(star_tracker_app);

  LoggingSimApp logger;
  sim.add_logging_app(logger);

  sim.run();

  return 0;
};

#endif