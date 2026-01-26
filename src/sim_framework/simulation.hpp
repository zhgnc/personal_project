#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "sim_control.hpp"
#include "sim_app_base.hpp"
#include "logging_app_base.hpp"
#include "sim_data_logger.hpp"
#include "sim_config.hpp"
#include "../utilities/yaml_utilities.hpp"
#include "../data_logging/logger.hpp"
#include "logging_facade.hpp"

#include <array>
#include <memory>
#include <cstdint>
#include <algorithm>
#include <chrono>
#include <iostream>

template<typename DataBusType>
class Simulation {
public:
    Simulation(const std::string& path_to_sim_config, DataBusType& bus);

    template<typename AppType>
    void add_app(AppType&& new_app);
    
    template<typename LoggingAppType>
    void add_logging_app(LoggingAppType&& new_logging_app);

    void run();

private:
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

    SimControl make_sim_control();
    void end_sim(const StopType& type, const StopReason& reason = StopReason::NotSpecified, const std::string& message = "None");
    void print_stop_diagnostics(const StopType& type, const StopReason& reason, const std::string& message);
    uint64_t get_next_seed();
    void update_accessible_sim_data();


    // ---- Configuration ----
    double start_time_sec;
    double config_stop_time_sec;
    uint64_t config_stop_time_usec;
    double sim_rate_hz;
    std::size_t num_mc_runs;
    uint64_t init_seed;
    bool print_hdf5_file_tree;
    bool print_file_attributes;

    std::string base_file_name;
    std::string data_output_directory;
    AppLoggingRates logging_rates;

    // ---- Runtime state ----
    StopType stop_type;
    StopReason stop_reason;
    std::string stop_message;
    double actual_stop_time_sec;

    std::size_t current_mc_run;
    uint64_t current_seed;
    double current_sim_time_sec;
    uint64_t current_sim_time_usec;
    uint64_t sim_dt_usec;
    double sim_dt_sec;
    uint64_t sim_step_count;

    // ---- Apps ----
    std::array<std::shared_ptr<SimAppBase<DataBusType>>,     SimConfig::max_app_number>         app_list;
    std::array<std::shared_ptr<LoggingAppBase<DataBusType>>, SimConfig::max_logging_app_number> logging_apps;

    std::size_t app_count;
    std::size_t logging_app_count;

    // ---- Logging ----
    std::unique_ptr<Logger> logger;
    std::unique_ptr<SimDataLogger> sim_data_logger;

    // ---- Timing ----
    static constexpr double sec2usec = 1e6;

    std::chrono::high_resolution_clock::time_point computer_start_time;
    std::chrono::high_resolution_clock::time_point computer_stop_time;
    std::chrono::duration<double> computer_elapsed_seconds;
    double sim_to_real_time_ratio;

    // ---- Shared data ----
    DataBusType& data_bus;
    SimMetaData meta_data;
    SimControl::AccessibleSimData accessible_sim_data;
};

#include "simulation.tpp"

#endif