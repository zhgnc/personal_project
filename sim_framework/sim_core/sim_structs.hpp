#ifndef SIM_STRUCTS_HPP
#define SIM_STRUCTS_HPP

#include <array>
#include <functional>
#include <memory>
#include <string>
#include <cstddef>

#include "../generic_apps/sim_app_base.hpp"
#include "sim_config.hpp"

// Stores a unique pointer to the original sim app (called `prototype`) and
// contains a function used to create independent copies of that app for each
// simulation run.
struct SimAppPrototype {
    SimAppPrototype() = default;

    // Store original app instance
    std::unique_ptr<SimAppBase> prototype;

    // Function that knows how to clone this specific type
    std::function<std::unique_ptr<SimAppBase>(const SimAppBase&)> copy_function;

    // Constructor saved the prototype object and builds a type-specific clone function
    template<typename AppType>
    SimAppPrototype(AppType&& original_app) {
        prototype = std::make_unique<std::decay_t<AppType>>(std::forward<AppType>(original_app));

        copy_function = [](const SimAppBase& base_app) -> std::unique_ptr<SimAppBase> {
            const std::decay_t<AppType>& derived_app = static_cast<const std::decay_t<AppType>&>(base_app);

            return std::make_unique<std::decay_t<AppType>>(derived_app);
        };
    }

    // Method used to create a unique copy of the derived app
    std::unique_ptr<SimAppBase> create() const {
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

struct SimSingleRunConfig {
    std::array<std::unique_ptr<SimAppBase>, SimConfig::max_app_number> apps;
    std::size_t app_count;

    // IO connection specs replayed into this run's own IoRegistry, and the
    // per-app telemetry settings (parallel to `apps`) applied by the recorder
    std::array<IoConnectionSpec, SimConfig::max_connection_number> io_connections;
    std::size_t io_connection_count = 0;

    std::array<AppTlmSettings, SimConfig::max_app_number> tlm_settings;

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
