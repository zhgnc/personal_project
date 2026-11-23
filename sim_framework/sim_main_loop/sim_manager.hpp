#ifndef SIM_MANAGER_HPP
#define SIM_MANAGER_HPP

#include <array>
#include <memory>
#include <string>
#include <cstdint>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#include <mutex>
#include <limits>
#include <stdexcept>

#include "sim_structs.hpp"
#include "sim_single_run.hpp"
#include "app_runner.hpp"
#include "../config_parsing/connection_yaml.hpp"
#include "../apps/app_class_list.hpp"
#include "../../utilities/yaml_utilities.hpp"

class SimManager {
public:
    // Builds every enabled app in the manifest named by the sim config's `app_manifest` key
    SimManager(const std::string& path_to_sim_config, const AppClassList& available_apps);

    void run();

private:
    void add_manifest_apps(const AppManifest& app_manifest, const AppClassList& available_apps,
                           const std::string& manifest_path);
    void sort_apps_by_priority();
    void load_app_connections();
    void add_app_connections(const AppConnections& app_connections);
    void check_connections_against_disabled_apps() const;
    void validate_apps();
    void run_monte_carlo_batch();
    static bool compare_by_priority(const SimAppPrototype& app_A, const SimAppPrototype& app_B);
    void thread_job();
    void display_sorted_app_info();
    void record_run_stats(const SimRunStats& run_stats);
    void display_run_summary();
    SimSingleRunConfig build_single_run_config(std::size_t run_number);

    // ---------------- App prototype storage ----------------
    std::array<SimAppPrototype, SimConfig::max_app_number> app_prototypes;

    std::size_t app_count;
    std::size_t num_threads;

    std::array<std::string, SimConfig::max_app_number> disabled_app_names;
    std::size_t disabled_app_count;
    std::mutex mutex;

    std::array<IoConfigSpec, SimConfig::max_connection_number> io_connection_specs;
    std::size_t io_connection_spec_count;

    // ---------------- Simulation config ----------------
    double start_time_sec;
    double stop_time_sec;
    double sim_rate_hz;
    std::size_t num_mc_runs;
    std::size_t current_run_number;
    uint64_t init_seed;

    std::string base_file_name;
    std::string output_directory;

    bool print_hdf5_file_tree;
    bool print_file_attributes;
    bool print_io_report;

    // Controls how much detail display_run_summary() prints after the batch:
    // 1 = completion message only, 2 = adds performance stats, >=3 = adds run
    // outcome breakdown, fastest/slowest runs, and output details
    uint8_t sim_stats_report_level;

    // ---------------- Batch summary statistics ----------------
    // Written by worker threads under `mutex` in record_run_stats(),
    // read without locking only after every worker has been joined
    std::size_t completed_run_count;
    std::size_t reached_stop_time_count;
    std::size_t num_threads_used;

    double total_sim_time_sec;
    double total_run_wall_clock_sec;
    double parallel_wall_clock_sec;
    double sim_to_real_time_ratio_sum;

    double fastest_run_wall_clock_sec;
    double slowest_run_wall_clock_sec;
    std::size_t fastest_run_number;
    std::size_t slowest_run_number;

    std::array<std::string, SimConfig::max_app_number> early_stop_reports;
    std::size_t early_stop_report_count;
};

#endif
