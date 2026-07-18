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

#include "sim_structs.hpp"
#include "sim_single_run.hpp"
#include "../messaging/connection_yaml.hpp"
#include "../../utilities/yaml_utilities.hpp"

class SimManager {
public:
    SimManager(const std::string& path_to_sim_config);

    // Accepts any object derived from `SimAppBase` and adds it to the
    // `app_prototypes`. The template parameter `AppType` preserves the
    // derived type so the sim can later create copies of the derived type
    // for each simulation run.
    template<typename AppType>
    void add_app(AppType&& new_app) {
        static_assert(std::is_base_of_v<SimAppBase, std::decay_t<AppType>>,
                      "AppType must derive from SimAppBase");

        if (app_count >= SimConfig::max_app_number) {
            throw std::runtime_error("[sim_manager.hpp] Number of apps exceeded value of `max_app_number` in sim_config.hpp which is "
                                    + std::to_string(SimConfig::max_app_number));
        }

        // Line wraps the `new_app` in a SimAppPrototype struct, which stores the original app
        // instance along with a function that can create independent copies for each MC run.
        // This is necessary because apps are stored polymorphically as `SimAppBase` pointers,
        // and C++ cannot automatically copy the original derived type through a base-class
        // pointer. `std::forward` preserves whether the app was passed as an lvalue or rvalue,
        // avoiding unnecessary copies while constructing the prototype.
        app_prototypes[app_count] = SimAppPrototype(std::forward<AppType>(new_app));
        app_count = app_count + 1;
    }

    // Connects a subscribed input to a published output, both given as
    // "app_name.port_name". Code-side alternative to the `connections:`
    // block in an app's config yaml, mainly useful for tests
    void connect(const std::string& destination, const std::string& source, const std::string& origin = "code");

    void run();

private:
    void load_app_io_configs();
    void validate_io_wiring();
    void sort_apps_by_priority();
    static bool compare_by_priority(const SimAppPrototype& app_A, const SimAppPrototype& app_B);
    void thread_job();
    void display_sorted_app_info();
    void record_run_stats(const SimRunStats& run_stats);
    void display_run_summary(double parallel_wall_clock_sec);
    SimSingleRunConfig build_single_run_config(std::size_t run_number);

    // ---------------- App prototype storage ----------------
    std::array<SimAppPrototype, SimConfig::max_app_number> app_prototypes;
    std::size_t app_count;

    // ---------------- IO connection specs and telemetry settings ----------------
    // Parsed once from the app config files (parallel to the sorted
    // app_prototypes for tlm_settings), then replayed into every run
    std::array<IoConnectionSpec, SimConfig::max_connection_number> io_connection_specs;
    std::size_t io_connection_spec_count;

    std::array<AppTlmSettings, SimConfig::max_app_number> tlm_settings;

    std::size_t num_threads;
    std::mutex mutex;

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
    double sim_to_real_time_ratio_sum;

    double fastest_run_wall_clock_sec;
    double slowest_run_wall_clock_sec;
    std::size_t fastest_run_number;
    std::size_t slowest_run_number;

    std::array<std::string, SimConfig::max_app_number> early_stop_reports;
    std::size_t early_stop_report_count;
};

#endif
