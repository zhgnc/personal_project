#ifndef SIM_SINGLE_RUN_HPP
#define SIM_SINGLE_RUN_HPP

#include <memory>
#include <string>
#include <chrono>
#include <iostream>

#include "sim_control.hpp"
#include "sim_config.hpp"
#include "sim_data_logger.hpp"
#include "sim_structs.hpp"
#include "../data_logging/logger.hpp"
#include "../telemetry/recorder.hpp"
#include "console_mutex.hpp"

class SimSingleRun {
public:
    // Takes ownership of the config object and moves its resources into the
    // sim run. An rvalue reference (`&&`) is used because the configuration
    // contains move-only objects like std::unique_ptr's
    SimSingleRun(SimSingleRunConfig&& config_data);

    SimRunStats run();

private:
    void setup();
    void setup_io();
    void step();
    void teardown();
    SimRunStats create_run_stats();

    void sim_teardown();
    SimControl make_ctrl(); // TODO: Refactor SimControl
    void end_sim(const StopType& type, const StopReason& reason, const std::string& message);
    void print_stop_diagnostics(const StopType& type, const StopReason& reason, const std::string& message);
    void log_run_meta_data();
    void update_accessible_sim_data();

    uint64_t get_seed();

    // Config data
    std::array<std::unique_ptr<SimAppBase>, SimConfig::max_app_number> apps;
    std::size_t app_count;

    // Heap-owned because the registry's fixed-size storage is too large for
    // a thread-stack local (see io_registry.hpp)
    std::unique_ptr<IoRegistry> io_registry;
    std::array<IoConnectionSpec, SimConfig::max_connection_number> io_connections;
    std::size_t io_connection_count;

    std::array<AppTlmSettings, SimConfig::max_app_number> tlm_settings;

    bool print_hdf5_file_tree;
    bool print_file_attributes;

    std::string output_directory;
    std::string base_file_name;

    double start_time_sec;
    double stop_time_sec;
    uint64_t stop_time_usec;
    double sim_rate_hz;
    double sim_dt_sec;
    uint64_t sim_dt_usec;

    std::size_t run_number;
    std::size_t total_mc_runs;

    uint64_t current_seed;
    uint64_t initial_seed;

    // Runtime data
    uint64_t current_sim_time_usec;
    double current_sim_time_sec;
    uint64_t sim_step_count;
    SimControl::AccessibleSimData accessible_sim_data;

    double actual_stop_time_sec;
    std::chrono::high_resolution_clock::time_point computer_start_time;
    std::chrono::high_resolution_clock::time_point computer_stop_time;
    std::chrono::duration<double> computer_elapsed_seconds;
    double sim_to_real_time_ratio;
    StopType stop_type;
    StopReason stop_reason;
    std::string stop_message;

    std::unique_ptr<Logger> logger;
    std::unique_ptr<SimDataLogger> sim_data_logger;

    SimMetaData meta_data;

    static constexpr double sec2usec = 1e6;
};

#endif
