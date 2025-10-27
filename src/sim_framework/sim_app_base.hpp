#ifndef SIM_APP_BASE_HPP
#define SIM_APP_BASE_HPP

#include <memory>
#include <cstdint>
#include <string>

class DataBus; // Forward declaration to speed up compile time

class SimAppBase {
public: 
    SimAppBase(double execution_rate_hz, 
           int schedule_priority, 
           const std::string& path_to_config);

    virtual void configure_model(const std::string& config_path, DataBus& data_bus) = 0;
    virtual void step() = 0;
    
    void initialize(DataBus& data_bus);
    void check_step(const uint32_t& sim_time_usec);

    int priority;
    double app_dt_sec;
    
private:   
    uint32_t app_dt_usec;
    uint32_t next_run_time_usec;    
    bool time_to_step;
    std::string config_path;

    double sec2usec = 1e6;
};

#endif