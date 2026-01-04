#include "../../src/data_logging/logger.hpp" 

void Logger::create_file(const std::string& full_file_path) {
    hdf5_file_ptr = log_utils.create_file(full_file_path);
};

void Logger::close_file() {
    for (const std::unique_ptr<DatasetBase>& dataset : datasets) {
        dataset->flush_buffer();
    }

    log_utils.close_file();
};

void Logger::add_group(const std::string& path_to_group) {
    log_utils.create_group(path_to_group);
};

void Logger::print_file_tree() {
    log_utils.print_file_tree();
};

void Logger::log_data(const uint32_t &sim_time_usec) {
    for (const std::unique_ptr<DatasetBase>& dataset : datasets) {
        dataset->log_if_needed(sim_time_usec);
    }
};