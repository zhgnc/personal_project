#ifndef LOGGING_APP_BASE_HPP
#define LOGGING_APP_BASE_HPP

#include <cstdint>
#include <string>

class DataBus; // Forward declaration to speed up compile time

class LoggingAppBase {
public: 
    LoggingAppBase() = default;
    LoggingAppBase(const std::string& config_file);

    void log_data(const uint32_t &sim_time_usec);
    virtual void initialize_logger(const std::string& file_name) = 0;
    virtual void data_to_log() = 0;

    std::string data_output_directory;
    
private:
    double logging_rate_hz; 
    double log_dt_sec;
    uint32_t log_dt_usec;
    bool time_to_step;
    
    double sec2usec = 1e6;
};

#endif