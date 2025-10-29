#include "../../src/sim_data_logger/sim_logging_manager.hpp" 


SimLoggingManager::SimLoggingManager(const std::string& config_file_path, const std::string& full_file_path) {
    YAML::Node config_file  = YAML::LoadFile(config_file_path);
    
    config_file    = config_file_path;
    hdf5_file_path = full_file_path;
    buffer_size    = config_file["buffer_length"].as<int>();
    hdf5_file_name = config_file["base_file_name"].as<std::string>();
};

void SimLoggingManager::create_file() {
    hdf5_file_ptr = log_utils.create_file(hdf5_file_path);
};

void SimLoggingManager::close_file() {
    log_utils.close_file();
};

void SimLoggingManager::add_group(const std::string& path_to_group) {
    log_utils.create_group(path_to_group);
};

void SimLoggingManager::print_file_tree() {
    log_utils.print_file_tree();
};

void SimLoggingManager::log_data() {

};

template<typename T>
void SimLoggingManager::add_dataset(const std::string& dataset_name, 
                                    const std::string& full_group_path, 
                                    std::shared_ptr<T> data_pointer, 
                                    int record_rate_hz) 
{
    log_utils.verify_file_path(full_group_path);
    dataset = DatasetOverrides(dataset_name, full_group_path, data_pointer, hdf5_file_ptr, record_rate_hz, buffer_size); 
    // Create dataset object which creates the dataset if it does not exist
    // Add dataset object to dataset list in class
}