#include "simulation.hpp"

#include <iostream>
#include "yaml-cpp/yaml.h"

Simulation::Simulation(const std::string& path_to_sim_config) {
    YAML::Node config_data = YAML::LoadFile(path_to_sim_config);

    start_time_sec             = config_data["sim_start_time_sec"].as<double>();
    stop_time_sec              = config_data["sim_stop_time_sec"].as<double>();
    simulation_rate_hz         = config_data["simulation_rate_hz"].as<double>();  
    logging_rate_hz            = config_data["logging_rate_hz"].as<double>();
    number_of_monte_carlo_runs = config_data["number_of_monte_carlo_runs"].as<int>();
    
    sim_dt_usec           = static_cast<uint32_t>(1e6 * (1.0 / simulation_rate_hz));
    current_sim_time_usec = static_cast<uint32_t>(1e6 * start_time_sec);
    current_sim_time_sec  = start_time_sec;
    stop_time_usec        = static_cast<uint32_t>(1e6 * stop_time_sec);
};

void Simulation::add_app(std::shared_ptr<SimApp> new_app) {
    app_list.push_back(new_app);  
};

bool Simulation::compare_by_priority(const std::shared_ptr<SimApp>& app_A, const std::shared_ptr<SimApp>& app_B) {
    // > sorts by descending  piority (higher number = higher priority)
    return app_A->schedule_priority > app_B->schedule_priority;
};

void Simulation::sort_apps_by_priority() {
    std::sort(app_list.begin(), app_list.end(), compare_by_priority);
};

void Simulation::initialize_apps() {
    std::cout << "[Simulation] Initializing Apps\n";
    for (std::shared_ptr<SimApp>& app : app_list) {
        app->initialize();
    }
    std::cout << "[Simulation] Apps Initialized\n\n";
}

void Simulation::run() {
    std::cout << "[Simulation] Starting Simulation\n";

    sort_apps_by_priority();
    initialize_apps();

    // Monte Carlo loop (basic version — just one run for now)
    for (int run_num = 0; run_num < number_of_monte_carlo_runs; run_num++) {
        std::cout << "[Simulation] Monte Carlo Run # Started: " << (run_num + 1) << "\n";

        while (current_sim_time_usec <= stop_time_usec) {
            for (auto& app : app_list) {
                app->step(current_sim_time_usec);
            }

            current_sim_time_usec += sim_dt_usec;
        }
        std::cout << "[Simulation] Monte Carlo Run # Ended: " << (run_num) << "\n";
    }
}