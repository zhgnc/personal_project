#ifndef SIM_LOGGING_MANAGER_HPP
#define SIM_LOGGING_MANAGER_HPP

#include "../../src/sim_data_logger/logging_utilities.hpp"
#include "../../src/sim_data_logger/dataset_overrides.hpp"
#include "../../src/sim_data_logger/dataset_base.hpp"

#include "yaml-cpp/yaml.h"

#include <string>
#include <vector>

class SimLoggingManager {
public:
    SimLoggingManager() = default;

    void create_file(const std::string& full_file_path);
    void close_file();
    void add_group(const std::string& path_to_group);
    void print_file_tree();
    void log_data(const uint32_t &sim_time_usec);

    // Since this function is a template function it needs to be implemented entirely in the header
    template<typename T>
    void add_dataset(const std::string& dataset_name, 
                     const std::string& full_group_path, 
                     const T& data_reference, 
                     const double record_rate_hz) 
    {
        log_utils.verify_group_exists(full_group_path);

        // [](const T*) {} ensures shared_ptr does not delete the referenced object
        std::shared_ptr<const T> data_pointer = std::shared_ptr<const T>(&data_reference, [](const T*) {}); 

        std::unique_ptr<DatasetBase> dataset  = std::make_unique<DatasetOverrides<T, buffer_length_config>>(
            dataset_name, full_group_path, data_pointer, hdf5_file_ptr, record_rate_hz); 
        
        dataset->create_dataset();
        datasets.push_back(std::move(dataset));
    }

private:
    LoggingUtilities log_utils;
    std::shared_ptr<H5::H5File> hdf5_file_ptr;
    std::vector<std::unique_ptr<DatasetBase>> datasets;
    static constexpr std::size_t buffer_length_config = 100; // TODO: Make this configurable
};

#endif