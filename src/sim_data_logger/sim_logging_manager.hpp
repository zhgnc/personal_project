#ifndef SIM_LOGGING_MANAGER_HPP
#define SIM_LOGGING_MANAGER_HPP

#include "../../src/sim_data_logger/logging_utilities.hpp"

#include <string>
#include <vector>

class SimLoggingManager {
public:
    SimLoggingManager() = default;
    SimLoggingManager(const std::string& config_file_path, const std::string& full_file_path);

    void create_file();
    void close_file();
    void add_group(const std::string& path_to_group);
    void print_file_tree();
    void log_data();

private:
    LoggingUtilities log_utils;
    std::string config_file; 
    std::string hdf5_file_path;
};

#endif