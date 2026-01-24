#ifndef SIM_APP_BASE_HPP
#define SIM_APP_BASE_HPP

#include <memory>
#include <cstdint>
#include <string>

#include "../../src/sim_framework/sim_control.hpp"

template<typename DataBusType>
class SimAppBase {
public: 
    SimAppBase(std::string app_name, 
               double execution_rate_hz, 
               int schedule_priority, 
               const std::string& path_to_config);

    virtual void configure_model(const std::string& path_to_config, SimulationControl& sim_ctrl) = 0;
    virtual void step(DataBusType& bus, SimulationControl& sim_ctrl) = 0;
    virtual void teardown(DataBusType& bus, SimulationControl& sim_ctrl) = 0;
    
    void initialize(SimulationControl& sim_ctrl);
    void check_step(const uint64_t& sim_time_usec, DataBusType& bus, SimulationControl& sim_ctrl);

    int priority;
    double app_dt_sec;
    std::string name;
    
private:   
    uint64_t app_dt_usec;
    uint64_t next_run_time_usec;    
    bool time_to_step;
    std::string config_path;

    double sec2usec = 1e6;
};

#include "sim_app_base.tpp"

#endif