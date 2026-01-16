#ifndef LOGGER_CONFIG_HPP
#define LOGGER_CONFIG_HPP

#include <array>

struct LoggerConfig {
    static constexpr std::size_t max_dataset_number    = 100;
    static constexpr std::size_t dataset_buffer_length = 1000;
};

#endif