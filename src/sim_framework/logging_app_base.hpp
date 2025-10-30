#ifndef LOGGING_APP_BASE_HPP
#define LOGGING_APP_BASE_HPP

#include "../../../src/sim_framework/data_bus.hpp"
#include "../../../src/sim_data_logger/sim_logging_manager.hpp"

#include <cstdint>
#include <string>

class DataBus; // Forward declaration to speed up compile time

class LoggingAppBase {
public: 
    LoggingAppBase(const std::string& config_file);

    void set_data_source(DataBus& bus);
    void log_data(const uint32_t &sim_time_usec);
    void create_new_file(const int& monte_carlo_run_number);
    virtual void configure_hdf5_file() = 0;

    SimLoggingManager logger;
    DataBus* data_bus;
private:
    std::string data_output_directory;
    std::string base_file_name;
    double logging_rate_hz; 
    double log_dt_sec;
    uint32_t log_dt_usec;
    bool time_to_step;
    std::string config_file_path;
    
    double sec2usec = 1e6;
};

#endif