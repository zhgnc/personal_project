#ifndef logger_HPP
#define logger_HPP

#include "../../src/data_logging/logging_utilities.hpp"
#include "../../src/data_logging/dataset_overrides.hpp"
#include "../../src/data_logging/dataset_base.hpp"

#include "yaml-cpp/yaml.h"

#include <string>
#include <vector>

class Logger {
public:
    Logger() = default;

    void create_file(const std::string& full_file_path);
    void close_file();
    void add_group(const std::string& path_to_group);
    void print_file_tree();
    void log_data(const uint32_t &sim_time_usec);

    template<typename T>
    void add_dataset(const std::string& dataset_name, 
                     const std::string& full_group_path, 
                     const T& data_reference, 
                     const double record_rate_hz);

private:
    LoggingUtilities log_utils;
    std::shared_ptr<H5::H5File> hdf5_file_ptr;
    std::vector<std::unique_ptr<DatasetBase>> datasets;
    static constexpr std::size_t buffer_length_config = 100; // TODO: Make this configurable
};

#include "logger.tpp"  // Template implementations
 

#endif