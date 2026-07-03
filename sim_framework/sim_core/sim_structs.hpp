#ifndef SIM_STRUCTS_HPP
#define SIM_STRUCTS_HPP

#include <array>
#include <memory>
#include <string>
#include <cstddef>

#include "sim_app_base.hpp"
#include "logging_app_base.hpp"
#include "sim_config.hpp"
#include "sim_control.hpp"

// Stores a unique pointer to the original sim app (called `prototype`) and contains 
// a function used to create independent copies of that app for each simulation run.
template<typename DataBusType>
struct SimAppPrototype {
    // Master copy of the app. This object is never executed directly and is
    // only used as the source for creating runtime instances.
    std::unique_ptr<SimAppBase<DataBusType>> prototype;

    std::unique_ptr<SimAppBase<DataBusType>>(*copy)(const SimAppBase<DataBusType>&);

    std::unique_ptr<SimAppBase<DataBusType>> create() const {
        return copy(*prototype);
    }
};

// Serves same function as `SimAppPrototype` execpt for the original logging apps 
template<typename DataBusType>
struct LoggingAppPrototype {
    std::unique_ptr<LoggingAppBase<DataBusType>> prototype;

    std::unique_ptr<LoggingAppBase<DataBusType>>(*copy)(const LoggingAppBase<DataBusType>&);

    std::unique_ptr<LoggingAppBase<DataBusType>> create() const {
        return copy(*prototype);
    }
};

template<typename DataBusType>
struct SimSingleRunConfig {
    DataBusType data_bus;

    std::array<std::unique_ptr<SimAppBase<DataBusType>>, SimConfig::max_app_number> apps;
    std::size_t app_count;

    std::array<std::unique_ptr<LoggingAppBase<DataBusType>>, SimConfig::max_logging_app_number> logging_apps;
    std::size_t logging_app_count;

    AppLoggingRates logging_rates;
    bool print_hdf5_file_tree;
    bool print_file_attributes;

    std::string output_directory;
    std::string base_file_name;

    double start_time_sec;
    double stop_time_sec;
    double sim_rate_hz;

    std::size_t run_number;
    std::size_t total_mc_runs;
    uint64_t initial_random_seed;
};

#endif