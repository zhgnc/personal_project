#include "../../src/sim_framework/logging_app_base.hpp" 
#include "yaml-cpp/yaml.h"
#include <format>


LoggingAppBase::LoggingAppBase(const std::string& config_file) {
    YAML::Node config_data = YAML::LoadFile(config_file);

    data_output_directory = config_data["save_data_directory"].as<std::string>();
    base_file_name        = config_data["base_file_name"].as<std::string>();

    logging_rate_hz       = config_data["logging_rate_hz"].as<double>();
    log_dt_sec            = 1.0 / logging_rate_hz;
    log_dt_usec           = static_cast<uint32_t>(sec2usec * log_dt_sec);
};

void LoggingAppBase::set_data_source(DataBus& bus) {
  data_bus = &bus;
}

void LoggingAppBase::log_data(const uint32_t &sim_time_usec) {
  time_to_step = sim_time_usec % log_dt_usec == 0;

  if (time_to_step == false) {
    return;
  }

  this->data_to_log();
};

void LoggingAppBase::create_new_file(const int& monte_carlo_run_number) {
  std::string new_file_name = std::format("{}_RUN_{:05}.hdf5", base_file_name, monte_carlo_run_number);
  logger = HDF5Logger(new_file_name, data_output_directory);
  logger.create_file();

  this->configure_hdf5_file();
}
