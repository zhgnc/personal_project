#ifndef SIM_CONFIG_HPP
#define SIM_CONFIG_HPP

#include <array>

// Class enables std::array instead of std::vector in sim_manager. The
// values in this class are arbitrary so they can be updated if needed
struct SimConfig {
    static constexpr std::size_t max_app_number         = 50;
    static constexpr std::size_t max_logging_app_number = 10;
    static constexpr std::size_t max_thread_number      = 32;

    // ---- IO registry limits (sim_framework/messaging) ----
    static constexpr std::size_t max_port_number         = 512;  // total declared ports per run
    static constexpr std::size_t max_connection_number   = 256;  // total connection requests per run
    static constexpr std::size_t max_app_input_number    = 64;   // subscribed inputs per app
    static constexpr std::size_t max_app_output_number   = 64;   // published outputs per app
    static constexpr std::size_t max_wiring_report_number = 64;  // stored wiring errors or warnings
    static constexpr std::size_t max_port_name_length    = 96;   // longest name compared for typo suggestions
};

#endif