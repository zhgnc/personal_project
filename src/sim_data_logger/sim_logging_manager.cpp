#include "../../src/sim_data_logger/sim_logging_manager.hpp" 

void SimLoggingManager::create_file(const std::string& full_file_path) {
    hdf5_file_ptr = log_utils.create_file(full_file_path);
};

void SimLoggingManager::close_file() {
    for (const std::unique_ptr<DatasetBase>& dataset : datasets) {
        dataset->flush_buffer();
    }

    log_utils.close_file();
};

void SimLoggingManager::add_group(const std::string& path_to_group) {
    log_utils.create_group(path_to_group);
};

void SimLoggingManager::print_file_tree() {
    log_utils.print_file_tree();
};

void SimLoggingManager::log_data(const uint32_t &sim_time_usec) {
    for (const std::unique_ptr<DatasetBase>& dataset : datasets) {
        dataset->log_if_needed(sim_time_usec);
    }
};