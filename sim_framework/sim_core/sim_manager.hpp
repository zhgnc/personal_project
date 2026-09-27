#ifndef SIM_MANAGER_HPP
#define SIM_MANAGER_HPP

// SimManager owns one Monte Carlo batch from end to end: it reads the
// simulation config and app manifest, instantiates the apps the manifest
// enables, validates the whole IO topology once before any thread starts,
// then runs the requested number of simulations across a thread pool and
// prints the batch summary.

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
#include "app_manifest.hpp"
#include "app_class_list.hpp"
#include "../messaging/connection_yaml.hpp"
#include "../../utilities/yaml_utilities.hpp"

// Top-level entry point for running a simulation. A project constructs one,
// hands it the sim config path and its list of available app classes, and
// calls run(); everything else comes from the config files.
//
// Both constructors parse the sim config and the app manifest immediately,
// so a broken config fails before run() is ever reached.
class SimManager {
public:
    // `available_apps` is the project's list of every sim app class the
    // manifest may instantiate (see app_class_list.hpp). C++ has no
    // reflection, so this list is what maps the manifest's `class:` strings
    // to constructors
    SimManager(const std::string& path_to_sim_config, const AppClassList& available_apps);

    // Test/tooling constructor: no manifest-instantiable classes, apps are
    // added in code with add_app()
    explicit SimManager(const std::string& path_to_sim_config);

    // Registers one app class outside an AppClassList, with an explicit
    // manifest name (useful for aliases and tests)
    template<typename AppType>
    void register_app_type(const std::string& class_name) {
        static_assert(std::is_base_of_v<SimAppBase, AppType>, "AppType must derive from SimAppBase");

        AppClassFactory factory;

        factory.class_name       = class_name;
        factory.create_prototype = [](const AppManifestEntry& entry, int schedule_priority) -> SimAppPrototype {
            return SimAppPrototype(AppType(entry.instance_name, entry.rate_hz, schedule_priority, entry.config_path));
        };

        add_class_factory(factory);
    }

    // Adds one already-constructed app, bypassing the manifest — the code
    // path used by tests. Telemetry settings are explicit arguments because
    // nothing may default silently.
    //
    // The template parameter `AppType` preserves the derived type so the sim
    // can later create copies of the derived type for each simulation run:
    // apps are stored polymorphically as `SimAppBase` pointers, and C++
    // cannot copy the derived type through a base-class pointer.
    template<typename AppType>
    void add_app(AppType&& new_app, TlmLevel tlm_level, double tlm_rate_hz) {
        static_assert(std::is_base_of_v<SimAppBase, std::decay_t<AppType>>,
                      "AppType must derive from SimAppBase");

        if (app_count >= SimConfig::max_app_number) {
            throw std::runtime_error(SimConfig::capacity_message("[sim_manager.hpp] number of apps",
                                                                 "max_app_number", SimConfig::max_app_number));
        }

        if (tlm_rate_hz <= 0.0) {
            throw std::runtime_error("[sim_manager.hpp] add_app('" + new_app.name() +
                                     "'): tlm_rate_hz must be greater than zero");
        }

        app_prototypes[app_count]     = SimAppPrototype(std::forward<AppType>(new_app));
        app_prototypes[app_count].tlm = AppTlmSettings{app_prototypes[app_count].prototype->name(),
                                                       tlm_rate_hz, tlm_level};
        app_count = app_count + 1;
    }

    // Runs the whole Monte Carlo batch: instantiate, validate, execute,
    // summarize. Throws before starting any run if the configuration is bad
    void run();

private:
    // Adds one class factory, rejecting a duplicate class name
    void add_class_factory(const AppClassFactory& factory);

    // ---- run() stages, in the order run() calls them ----
    void instantiate_manifest_apps();             // manifest entries -> app prototypes
    void sort_apps_by_priority();                 // frame order, lowest priority number first
    void load_app_io_configs();                   // each app's input_mapping -> connection specs
    void check_connections_against_disabled_apps();  // clearer error than "app does not exist"
    void validate_io_wiring();                    // resolve the whole topology once, print the report
    void thread_job();                            // worker loop: claim a run number, execute it

    static bool compare_by_priority(const SimAppPrototype& app_a, const SimAppPrototype& app_b);
    void display_sorted_app_info();
    void record_run_stats(const SimRunStats& run_stats);
    void display_run_summary(double parallel_wall_clock_sec);

    // Copies everything one run needs into a self-contained config, including
    // freshly cloned apps, so runs share no mutable state
    SimSingleRunConfig build_single_run_config(std::size_t run_number);

    // ---------------- App prototype storage ----------------
    std::array<SimAppPrototype, SimConfig::max_app_number> app_prototypes;
    std::size_t app_count;

    // ---------------- App manifest and class factories ----------------
    // Manifest entries in file order (which defines execution order);
    // factories map manifest class names to constructors
    std::array<AppManifestEntry, SimConfig::max_app_number> manifest_entries;
    std::size_t manifest_entry_count;

    std::array<AppClassFactory, SimConfig::max_app_number> app_class_factories;
    std::size_t app_class_factory_count;

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
