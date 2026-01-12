#ifndef SIM_MANAGER_HPP
#define SIM_MANAGER_HPP

#include "sim_single_run.hpp"
#include "sim_control.hpp"
#include "sim_app_base.hpp"
#include "logging_app_base.hpp"
#include "sim_data_logger.hpp"
#include "../utilities/yaml_utilities.hpp"
#include "../data_logging/logger.hpp"

#include <vector>
#include <memory>
#include <string>
#include <cstdint>
#include <thread>
#include <future>
#include <iostream>

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
    void run_single_mc(const SimSingleRunConfig& config);
    void sort_apps_by_priority();
    void manager_teardown();
    void display_sorted_app_info();
    static bool compare_by_priority(const std::shared_ptr<SimAppBase<DataBusType>>& app_A, 
                                    const std::shared_ptr<SimAppBase<DataBusType>>& app_B);
    
    DataBusType& data_bus;
    SimSingleRunConfig run_config;
    Logger logger;

    std::vector<std::shared_ptr<SimAppBase<DataBusType>>> app_list;
    std::vector<std::shared_ptr<LoggingAppBase<DataBusType>>> logging_apps;
    
    std::size_t num_mc_runs;
    std::size_t num_parallel_processors;
    std::size_t init_seed;
    double sim_rate_hz;
    bool print_hdf5_file_tree;
    bool print_file_attributes;

    std::string data_output_directory;
    std::string base_file_name;
    AppLoggingRates logging_rates;

    double start_time_sec;
    double config_stop_time_sec;

    uint64_t sim_dt_usec;
    uint64_t current_sim_time_usec;
    double current_sim_time_sec;
    uint64_t config_stop_time_usec;
};

#include "sim_manager.tpp"

#endif