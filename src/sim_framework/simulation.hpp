#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "sim_control.hpp"
#include "sim_app_base.hpp"
#include "logging_app_base.hpp"
#include "sim_data_logger.hpp"
#include "sim_config.hpp"
#include "../utilities/yaml_utilities.hpp"
#include "../data_logging/logger.hpp"

#include <array>
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
    void initialize_pre_run_data();
    void log_run_meta_data();

    void end_sim_after_cycle(const StopReason& reason = StopReason::NotSpecified, const std::string& message = "None") override;
    void end_sim_after_app(const StopReason& reason = StopReason::NotSpecified, const std::string& message = "None") override;
    bool stop_requested() const override;
    void print_stop_diagnostics(const StopType& type, const StopReason& reason, const std::string& message);
    uint64_t get_next_seed() override;
    const AccessibleSimData& public_sim_data() const;
    void update_accessible_sim_data();


    double start_time_sec;
    double config_stop_time_sec;
    double sim_rate_hz;
    std::size_t num_mc_runs;
    uint64_t init_seed;
    uint64_t current_seed;
    bool print_hdf5_file_tree;
    bool print_file_attributes;
    
    bool stop_sim_now;
    bool stop_sim_after_cycle;      
    StopType stop_type;          
    StopReason stop_reason;  
    std::string stop_message;
    double actual_stop_time_sec;

    std::array<std::shared_ptr<SimAppBase<DataBusType>>, SimConfig::max_app_number> app_list;
    std::array<std::shared_ptr<LoggingAppBase<DataBusType>>, SimConfig::max_logging_app_number> logging_apps;
    std::unique_ptr<SimDataLogger> sim_data_logger;
    std::unique_ptr<Logger> logger;

    std::size_t app_count         = 0;
    std::size_t logging_app_count = 0;


    AppLoggingRates logging_rates;
    std::string base_file_name;
    std::string data_output_directory;
    
    std::size_t current_mc_run;
    double current_sim_time_sec;
    uint64_t current_sim_time_usec;
    uint64_t config_stop_time_usec;
    uint64_t sim_dt_usec;
    double sim_dt_sec;
    uint64_t sim_step_count;

    double sec2usec = 1e6;

    std::chrono::high_resolution_clock::time_point computer_start_time;
    std::chrono::high_resolution_clock::time_point computer_stop_time;
    std::chrono::duration<double> computer_elapsed_seconds; 
    double sim_to_real_time_ratio;

    DataBusType& data_bus;
    SimMetaData meta_data;
    AccessibleSimData accessible_sim_data;
};

#include "simulation.tpp"

#endif