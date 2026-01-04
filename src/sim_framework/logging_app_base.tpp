#include "../../src/sim_framework/logging_app_base.hpp" 
#include "yaml-cpp/yaml.h"
#include <format>

template<typename DataBusType>
LoggingAppBase<DataBusType>::LoggingAppBase(const std::string& config_file) {
    YAML::Node config_data = YAML::LoadFile(config_file);

    data_output_directory  = config_data["save_data_directory"].as<std::string>();
    base_file_name         = config_data["base_file_name"].as<std::string>();
};

template<typename DataBusType>
void LoggingAppBase<DataBusType>::set_data_source(DataBusType& bus) {
  data_bus = &bus;
}

template<typename DataBusType>
void LoggingAppBase<DataBusType>::create_new_file(const int& monte_carlo_run_number) {
  std::string new_file_name = std::format("{}_RUN_{:05}.hdf5", base_file_name, monte_carlo_run_number);
  
  std::string full_path = data_output_directory + "/" + new_file_name;

  logger = Logger();
  logger.create_file(full_path);

  this->configure_hdf5_file();
}

template<typename DataBusType>
void LoggingAppBase<DataBusType>::log_data(const uint32_t &sim_time_usec) {
  logger.log_data(sim_time_usec);
};

template<typename DataBusType>
void LoggingAppBase<DataBusType>::close_file() {
  logger.close_file();
}
