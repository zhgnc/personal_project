#ifndef SIM_STRUCTS_HPP
#define SIM_STRUCTS_HPP

#include <array>
#include <memory>
#include <string>
#include <cstddef>

#include "../generic_apps/sim_app_base.hpp"
#include "../generic_apps/logging_app_base.hpp"
#include "sim_config.hpp"

// Stores a unique pointer to the original sim app (called `prototype`) and
// contains a function used to create independent copies of that app for each
// simulation run.
template<typename DataBusType>
struct SimAppPrototype {
    SimAppPrototype() = default;

    // Store original app instance
    std::unique_ptr<SimAppBase<DataBusType>> prototype;

    // Function that knows how to clone this specific type
    std::function<std::unique_ptr<SimAppBase<DataBusType>>(const SimAppBase<DataBusType>&)> copy_function;

    // Constructor saved the prototype object and builds a type-specific clone function
    template<typename AppType>
    SimAppPrototype(AppType&& original_app) {
        prototype = std::make_unique<std::decay_t<AppType>>(std::forward<AppType>(original_app));

        copy_function = [](const SimAppBase<DataBusType>& base_app) -> std::unique_ptr<SimAppBase<DataBusType>> {
            const std::decay_t<AppType>& derived_app = static_cast<const std::decay_t<AppType>&>(base_app);
            
            return std::make_unique<std::decay_t<AppType>>(derived_app);
        };
    }

    // Method used to create aunique copy of the derived app
    std::unique_ptr<SimAppBase<DataBusType>> create() const {
        return copy_function(*prototype);
    }
};

// Serves same function as `SimAppPrototype` but for logging apps derived from LoggingAppType
template<typename DataBusType>
struct LoggingAppPrototype {
    LoggingAppPrototype() = default;

    std::unique_ptr<LoggingAppBase<DataBusType>> prototype;

    std::function<std::unique_ptr<LoggingAppBase<DataBusType>>(const LoggingAppBase<DataBusType>&)> copy_function;

    template<typename LoggingAppType>
    LoggingAppPrototype(LoggingAppType&& original_app) {
        prototype = std::make_unique<std::decay_t<LoggingAppType>>(std::forward<LoggingAppType>(original_app));

        copy_function = [](const LoggingAppBase<DataBusType>& base_app) -> std::unique_ptr<LoggingAppBase<DataBusType>> {
            const std::decay_t<LoggingAppType>& derived_app = static_cast<const std::decay_t<LoggingAppType>&>(base_app);

            return std::make_unique<std::decay_t<LoggingAppType>>(derived_app);
        };
    }

    std::unique_ptr<LoggingAppBase<DataBusType>> create() const {
        return copy_function(*prototype);
    }
};

struct SimRunStats {
    std::size_t run_number;
    double sim_time_sec;            
    double wall_clock_sec;          
    double sim_to_real_time_ratio;
    StopType stop_type;             
    StopReason stop_reason;
    std::string stop_message;
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