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

template<typename DataBusType>
struct SimSingleRunConfig {
    DataBusType data_bus;

    // Array of factory functions that create simulation apps. Each function returns a new unique_ptr 
    // instance of a SimAppBase-derived type.
    std::array<std::function<std::unique_ptr<SimAppBase<DataBusType>>()>, SimConfig::max_app_number> apps;
    std::size_t app_count;

    std::array<std::function<std::unique_ptr<LoggingAppBase<DataBusType>>()>, SimConfig::max_logging_app_number> logging_apps;
    std::size_t logging_app_count;

    AppLoggingRates logging_rates;

    std::string output_directory;
    std::string base_file_name;

    double start_time_sec;
    double stop_time_sec;
    double sim_rate_hz;

    std::size_t run_number;
};

#endif