#ifndef SIM_APP_HPP
#define SIM_APP_HPP

#include "sim_app_base_interface.hpp"
#include <memory>
#include <cstdint>

class SimApp {
public: 
    SimApp(std::shared_ptr<SimAppBaseInterface> pointer_to_app, double execution_rate_hz, int schedule_priority);

    void initialize();
    void step(const uint32_t& sim_time_usec);

    int schedule_priority;

    
private:
    std::shared_ptr<SimAppBaseInterface> app;
    
    double   app_dt_sec;
    uint32_t app_dt_usec;
    uint32_t next_run_time_usec;    
    bool     time_to_step;
};

#endif