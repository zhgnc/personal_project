#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "sim_control.hpp"
#include "sim_app_base.hpp"
#include "logging_app_base.hpp"
#include "sim_data_logger.hpp"
#include "../utilities/yaml_utilities.hpp"
#include "../data_logging/logger.hpp"

#include <vector>
#include <memory>
#include <cstdint>
#include <algorithm>
#include <chrono>
#include <iostream>

template<typename DataBusType>
class Simulation : SimulationControl {
public:
    Simulation(const std::string& path_to_sim_config, DataBusType& bus);

    template<typename AppType>
    void add_app(AppType&& new_app);
    
    template<typename LoggingAppType>
    void add_logging_app(LoggingAppType&& new_logging_app);

    void run();
    void stop_sim(StopReason reason = StopReason::Unknown, const std::string& message = "None") override;

private:
    void initialize_apps();
    void sort_apps_by_priority();
    void display_sorted_app_info();
    static bool compare_by_priority(const std::shared_ptr<SimAppBase<DataBusType>>& app_A, 
                                    const std::shared_ptr<SimAppBase<DataBusType>>& app_B);
    void run_setup(std::size_t run_num);
    void run_step();
    void run_teardown();
    void sim_teardown();

    double start_time_sec;
    double config_stop_time_sec;
    double sim_rate_hz;
    std::size_t num_mc_runs;
    bool print_hdf5_file_tree;
    bool print_file_attributes;
    
    bool end_sim;                
    StopReason stop_reason;  
    std::string stop_message;
    double actual_stop_time_sec;

    std::vector<std::shared_ptr<SimAppBase<DataBusType>>> app_list;
    std::vector<std::shared_ptr<LoggingAppBase<DataBusType>>> logging_apps;
    std::unique_ptr<SimDataLogger> sim_data_logger;
    
    Logger logger;
    AppLoggingRates logging_rates;
    std::string base_file_name;
    std::string data_output_directory;
    
    std::size_t current_mc_run;
    double current_sim_time_sec;
    uint64_t current_sim_time_usec;
    uint64_t config_stop_time_usec;
    uint64_t sim_dt_usec;

    double sec2usec = 1e6;

    std::chrono::high_resolution_clock::time_point computer_start_time;
    std::chrono::high_resolution_clock::time_point computer_stop_time;
    std::chrono::duration<double> computer_elapsed_seconds; 
    double sim_to_real_time_ratio;

    DataBusType& data_bus;
};

#include "simulation.tpp"

#endif