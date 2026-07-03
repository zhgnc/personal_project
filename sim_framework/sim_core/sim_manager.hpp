#ifndef SIM_MANAGER_HPP
#define SIM_MANAGER_HPP

#include <array>
#include <memory>
#include <string>
#include <cstdint>
#include <algorithm>
#include <chrono>
#include <iostream>

#include "sim_structs.hpp"
#include "sim_single_run.hpp"

template<typename DataBusType>
class SimManager {
public:
    SimManager(const std::string& path_to_sim_config, DataBusType& bus);

    template<typename AppType>
    void add_app(AppType&& new_app);

    template<typename LoggingAppType>
    void add_logging_app(LoggingAppType&& new_logging_app);

    void run();

private:
    uint64_t get_seed(); // TODO: Delete
    void sort_apps_by_priority();
    bool compare_by_priority(const std::shared_ptr<SimAppBase<DataBusType>> &app_A, 
                             const std::shared_ptr<SimAppBase<DataBusType>> &app_B);
    void display_sorted_app_info();


    SimSingleRunConfig<DataBusType> build_single_run_config(const std::size_t& run_number);
    
    // ---------------- App and logger prototype storage ----------------
    std::array<SimAppPrototype<DataBusType>, SimConfig::max_app_number> app_prototypes;
    std::array<LoggingAppPrototype<DataBusType>, SimConfig::max_logging_app_number> logging_prototypes;

    std::size_t app_count;
    std::size_t logging_app_count;

    // ---------------- Simulation config ----------------
    double start_time_sec;
    double stop_time_sec;
    double sim_rate_hz;
    std::size_t num_mc_runs;
    uint64_t init_seed;

    std::string base_file_name;
    std::string output_directory;
    AppLoggingRates logging_rates;

    bool print_hdf5_file_tree; 
    bool print_file_attributes;

    DataBusType& data_bus;
};

#include "sim_manager.tpp"

#endif