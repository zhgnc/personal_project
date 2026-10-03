#ifndef SIM_CONFIG_HPP
#define SIM_CONFIG_HPP

#include <cstddef>
#include <string>

// Every fixed-capacity limit in the sim framework lives here. The framework
// uses `std::array` + a count member variable instead of `std::vector` so no
// container allocates during a run and these constants are the capacities
// of those arrays. The values are arbitrary and safe to raise and if the
// capacity is exceeded the result is a startup error that names the constant
// to increase (see `capacity_message()`).
//
// This header sits at the framework root rather than inside a folder because
// every part of the framework depends on it
struct SimConfig {
    static constexpr std::size_t max_app_number    = 50;  // Apps in one simulation
    static constexpr std::size_t max_thread_number = 32;  // Monte Carlo worker threads

    // Removed when the logging apps are replaced by the telemetry recorder
    static constexpr std::size_t max_logging_app_number = 10;

    static constexpr std::size_t max_port_number          = 512;  // Declared ports per run, all apps
    static constexpr std::size_t max_connection_number    = 256;  // Input connections per run, all apps
    static constexpr std::size_t max_app_input_number     = 64;   // Subscribed inputs for one app
    static constexpr std::size_t max_app_output_number    = 64;   // Published ports for one app
    static constexpr std::size_t max_wiring_report_number = 64;   // Stored errors or warnings per report

    // Standard text for a fixed-capacity overflow. `what` names the thing that
    // did not fit (e.g. "declaring app 'gyro_app'"), `limit_name` is the
    // constant above that must be raised, and the `limit` is the current value
    static std::string capacity_message(const std::string& what, const char* limit_name, std::size_t limit) {
        return what + " exceeds SimConfig::" + limit_name + " (" + std::to_string(limit) +
               ")!!!\n Increase the limit in sim_config.hpp!!!";
    }
};

#endif
