#ifndef SIM_CONFIG_HPP
#define SIM_CONFIG_HPP

#include <array>

struct SimConfig {
    static constexpr std::size_t max_app_number         = 50;
    static constexpr std::size_t max_logging_app_number = 10;
};

#endif