#include "../../src/sim_framework/logging_app_base.hpp" 
#include "yaml-cpp/yaml.h"
#include <format>


LoggingAppBase::LoggingAppBase(const std::string& config_file) {
    YAML::Node config_data = YAML::LoadFile(config_file);

    data_output_directory  = config_data["save_data_directory"].as<std::string>();
    base_file_name         = config_data["base_file_name"].as<std::string>();

    logging_rate_hz        = config_data["logging_rate_hz"].as<double>();
    log_dt_sec             = 1.0 / logging_rate_hz;
    log_dt_usec            = static_cast<uint32_t>(sec2usec * log_dt_sec);

    config_file_path       = config_file;
};

void LoggingAppBase::set_data_source(DataBus& bus) {
  data_bus = &bus;
}

void LoggingAppBase::log_data(const uint32_t &sim_time_usec) {
  logger.log_data(sim_time_usec);
};

void LoggingAppBase::create_new_file(const int& monte_carlo_run_number) {
  std::string new_file_name = std::format("{}_RUN_{:05}.hdf5", base_file_name, monte_carlo_run_number);
  
  std::string full_path = data_output_directory + "/" + new_file_name;

  logger = SimLoggingManager();
  logger.create_file(full_path);

  this->configure_hdf5_file();
}
