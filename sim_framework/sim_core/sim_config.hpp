#ifndef SIM_CONFIG_HPP
#define SIM_CONFIG_HPP

#include <array>

// Class enables std::array instead of std::vector in sim_manager. The 
// values in this class are arbitrary so they can be updated if needed
struct SimConfig {
    static constexpr std::size_t max_app_number         = 50; 
    static constexpr std::size_t max_logging_app_number = 10; 
    static constexpr std::size_t max_thread_number      = 32; 
};

#endif