#include "logger.hpp"

template<typename T>
void Logger::add_dataset(const std::string& dataset_name, 
                         const std::string& full_group_path, 
                         const T& data_reference, 
                         const double record_rate_hz) 
{
    verify_group_exists(full_group_path);

    // [](const T*) {} ensures shared_ptr does not delete the referenced object
    std::shared_ptr<const T> data_pointer = std::shared_ptr<const T>(&data_reference, [](const T*) {}); 

    std::unique_ptr<DatasetBase> dataset  = std::make_unique<DatasetOverrides<T, buffer_length_config>>(
        dataset_name, full_group_path, data_pointer, hdf5_file_ptr, record_rate_hz); 
    
    dataset->create_dataset();
    datasets.push_back(std::move(dataset));
};