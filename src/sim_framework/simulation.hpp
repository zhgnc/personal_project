#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "sim_app_base.hpp"
#include "data_bus.hpp"
#include "logging_app_base.hpp"

#include <vector>
#include <memory>
#include <cstdint>
#include <algorithm>
#include <chrono>

class Simulation {
public:
    Simulation(const std::string& path_to_sim_config, DataBus& data_bus);

    void add_app(std::shared_ptr<SimAppBase> new_app);
    void add_logger(std::shared_ptr<LoggingAppBase> logger);
    void run();

private:
    void initialize_apps();
    void sort_apps_by_priority();
    static bool compare_by_priority(const std::shared_ptr<SimAppBase>& app_A, 
                                    const std::shared_ptr<SimAppBase>& app_B);
    void display_run_status_time(const int& run_number);

    double start_time_sec;
    double stop_time_sec;
    double sim_rate_hz;
    double logging_rate_hz;
    std::size_t num_mc_runs;

    std::vector<std::shared_ptr<SimAppBase>> app_list;
    std::shared_ptr<LoggingAppBase> data_logger;

    double current_sim_time_sec;
    uint32_t current_sim_time_usec;
    uint32_t stop_time_usec;
    uint32_t sim_dt_usec;

    double sec2usec = 1e6;

    std::chrono::high_resolution_clock::time_point computer_start_time;
    std::chrono::high_resolution_clock::time_point computer_stop_time;
    std::chrono::duration<double> computer_elapsed_seconds; 
    double sim_to_real_time;

    DataBus data_bus;
};

#endif