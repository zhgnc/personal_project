#ifndef LOGGING_APP_BASE_HPP
#define LOGGING_APP_BASE_HPP

#include "../../src/data_logging/logger.hpp"
#include "../../src/utilities/yaml_utilities.hpp"

#include <format>
#include <cstdint>
#include <string>

template<typename DataBusType>
class LoggingAppBase {
public: 
    LoggingAppBase(const std::string& config_file);

    void set_data_source(DataBusType& bus);
    void log_data(const uint64_t &sim_time_usec);
    void create_new_file(const int& monte_carlo_run_number);
    void close_file();
    virtual void configure_hdf5_file() = 0;

    Logger logger;
    DataBusType* data_bus;
private:
    std::string data_output_directory;
    std::string base_file_name;
};

#include "logging_app_base.tpp"

#endif