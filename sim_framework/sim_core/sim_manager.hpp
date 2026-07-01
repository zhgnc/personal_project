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
#include "sim_config.hpp"

template<typename DataBusType>
class SimManager {
public:
    SimManager(const std::string& path_to_sim_config,
                      DataBusType& bus);

    // User API: add apps WITHOUT knowing about prototypes
    template<typename AppType>
    void add_app(AppType&& app);

    template<typename LoggingAppType>
    void add_logging_app(LoggingAppType&& app);

    void run();

private:

    // ---------------- Prototype storage ----------------
    std::array<SimAppPrototype<DataBusType>, SimConfig::max_app_number> app_prototypes;
    std::array<LoggingAppPrototype<DataBusType>, SimConfig::max_logging_app_number> logging_prototypes;

    std::size_t app_count = 0;
    std::size_t logging_app_count = 0;

    // ---------------- Simulation config ----------------
    double start_time_sec;
    double stop_time_sec;
    double sim_rate_hz;
    std::size_t num_mc_runs;
    uint64_t init_seed;

    std::string base_file_name;
    std::string output_directory;
    AppLoggingRates logging_rates;

    // ---------------- Runtime state ----------------
    uint64_t current_seed;
    std::size_t current_mc_run = 0;

    DataBusType& data_bus;

    // ---------------- Helpers ----------------
    uint64_t get_seed();
    void build_single_run_config(SimSingleRunConfig<DataBusType>& cfg);
};

#include "sim_manager.tpp"

#endif