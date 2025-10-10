#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "sim_app.hpp"

#include <vector>
#include <memory>
#include <cstdint>
#include <algorithm>

class Simulation {
public:
    Simulation(const std::string& path_to_sim_config);

    void add_app(std::shared_ptr<SimApp> new_app);
    void run();

private:
    void initialize_apps();
    void sort_apps_by_priority();
    bool compare_by_priority(const std::shared_ptr<SimApp>& app_A, 
                             const std::shared_ptr<SimApp>& app_B);

    double start_time_sec;
    double stop_time_sec;
    double simulation_rate_hz;
    double logging_rate_hz;
    int    number_of_monte_carlo_runs;

    std::vector<std::shared_ptr<SimApp>> app_list;
    uint32_t current_sim_time_usec;
    double current_sim_time_sec;
    uint32_t stop_time_usec;
    uint32_t sim_dt_usec;
};

#endif