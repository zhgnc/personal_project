#ifndef SIM_APP_BASE_HPP
#define SIM_APP_BASE_HPP

#include <memory>
#include <cstdint>
#include <string>

#include "../../src/sim_framework/sim_control.hpp"
#include "sim_control.hpp"

// Forward declare template Simulation so that it can be a friend class. This 
// allows the `simulation` class to access the `initialize` and `check_step` 
// functions without exposing the methods to the user when creating their own 
// apps by inheriting from the `SimAppBase` class  
template<typename DataBusType>
class Simulation;


template<typename DataBusType>
class SimAppBase {
public: 
    SimAppBase(std::string app_of_name, 
               double execution_rate_hz, 
               int schedule_priority, 
               const std::string& path_to_config);

    virtual void configure_model(const std::string& path_to_config, SimControl& sim_ctrl) = 0;
    virtual void step(DataBusType& bus, SimControl& sim_ctrl) = 0;
    virtual void teardown(DataBusType& bus, SimControl& sim_ctrl) = 0;

    const std::string& name() const { return app_name; }
    int priority() const { return app_priority; }
    double dt_sec() const { return app_dt_sec; }
    
private:   
    friend class Simulation<DataBusType>;
    
    void initialize(SimControl& sim_ctrl);
    void check_step(const uint64_t& sim_time_usec, DataBusType& bus, SimControl& sim_ctrl);
    
    uint64_t app_dt_usec;
    bool time_to_step;
    std::string config_path;
    int app_priority;
    double app_dt_sec;
    std::string app_name;

    static constexpr double sec2usec = 1e6;
};

#include "sim_app_base.tpp"

#endif