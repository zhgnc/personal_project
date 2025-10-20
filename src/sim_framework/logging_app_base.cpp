#include "../../src/sim_framework/logging_app_base.hpp" 
#include "yaml-cpp/yaml.h"


LoggingAppBase::LoggingAppBase(const std::string& config_file) {
    YAML::Node config_data = YAML::LoadFile(config_file);

    data_output_directory = config_data["save_data_directory"].as<std::string>();
    logging_rate_hz       = config_data["fastest_logging_rate_hz"].as<double>();
    log_dt_sec            = 1.0 / logging_rate_hz;
    log_dt_usec           = static_cast<uint32_t>(sec2usec * log_dt_sec);
};

void LoggingAppBase::log_data(const uint32_t &sim_time_usec) {
  time_to_step = sim_time_usec % log_dt_usec == 0;

  if (time_to_step == false) {
    return;
  }

  this->data_to_log();
};
