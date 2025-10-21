#ifndef LOGGING_APP_BASE_HPP
#define LOGGING_APP_BASE_HPP

#include "../../../src/data_logger/hdf5_logger.hpp"

#include <cstdint>
#include <string>

class DataBus; // Forward declaration to speed up compile time

class LoggingAppBase {
public: 
    LoggingAppBase(const std::string& config_file);

    void log_data(const uint32_t &sim_time_usec);
    void create_new_file(const int& monte_carlo_run_number);
    virtual void configure_hdf5_file() = 0;
    virtual void data_to_log() = 0;

    HDF5Logger logger;
private:
    std::string data_output_directory;
    std::string base_file_name;
    double logging_rate_hz; 
    double log_dt_sec;
    uint32_t log_dt_usec;
    bool time_to_step;
    
    double sec2usec = 1e6;
};

#endif