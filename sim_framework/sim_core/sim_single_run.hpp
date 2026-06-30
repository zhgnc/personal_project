#ifndef SIM_SINGLE_RUN_HPP
#define SIM_SINGLE_RUN_HPP

#include <memory>
#include <string>
#include <chrono>
#include <iostream>

#include "sim_control.hpp"
#include "sim_config.hpp"
#include "sim_data_logger.hpp"
#include "sim_structs.hpp"
#include "../data_logging/logger.hpp"

template<typename DataBusType>
class SimSingleRun {
public:
    SimSingleRun(const SimSingleRunConfig<DataBusType>& config_data);

    void run();

private:
    void setup(); 
    void step();
    void teardown();

    SimSingleRunConfig<DataBusType> config;

    std::array<std::unique_ptr<SimAppBase<DataBusType>>, SimConfig::max_app_number> apps;
    std::array<std::unique_ptr<LoggingAppBase<DataBusType>>, SimConfig::max_logging_app_number> logging_apps;

    // ---------------- Runtime state ----------------
    uint64_t current_sim_time_usec;
    double current_sim_time_sec;
    uint64_t sim_step_count;
    uint64_t current_seed;
    uint64_t sim_dt_usec;
    SimControl::AccessibleSimData accessible_sim_data;

    // ---------------- Stop state ----------------
    StopType stop_type;
    StopReason stop_reason;
    std::string stop_message;
    uint64_t stop_time_usec;

    // ---------------- Runtime objects ----------------
    std::unique_ptr<Logger> logger;
    std::unique_ptr<SimDataLogger> sim_data_logger;

    SimControl make_ctrl();

    static constexpr double sec2usec = 1e6;
};

#include "sim_single_run.tpp"

#endif