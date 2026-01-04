#ifndef DATASET_OVERRIDES_TPP
#define DATASET_OVERRIDES_TPP

#include "../../src/data_logging/dataset_overrides.hpp"
#include "../../external/hdf5/include/H5Cpp.h"
#include "../../src/math_utilities/math.hpp"

#include <iostream>
#include <stdexcept>

// Implementation of DataTraits struct methods
template<typename T>
std::array<hsize_t, 0> DataTraits<T>::dimension_sizes() { return {}; }

template<typename T>
const void* DataTraits<T>::data_ptr(const T* value) { return value; }


template<typename T, size_t N>
std::array<hsize_t, 1> DataTraits<vector<T, N>>::dimension_sizes() { return {N}; }

template<typename T, size_t N>
const void* DataTraits<vector<T, N>>::data_ptr(const vector<T, N>* vec) { return vec->data(); }


template<typename T, size_t R, size_t C>
std::array<hsize_t, 2> DataTraits<matrix<T, R, C>>::dimension_sizes() { return {R, C}; }

template<typename T, size_t R, size_t C>
const void* DataTraits<matrix<T, R, C>>::data_ptr(const matrix<T, R, C>* mat) { return mat->data(); }





template<typename T, std::size_t buffer_length>
DatasetOverrides<T, buffer_length>::DatasetOverrides(const std::string& name,
                                                     const std::string& full_group_path,
                                                     std::shared_ptr<const T> data_pointer,
                                                     std::shared_ptr<H5::H5File> file,
                                                     const double record_rate_hz)
{
    dataset_name    = name;
    group_path      = full_group_path;
    logging_rate    = record_rate_hz;
    logging_dt_usec = static_cast<uint32_t>(sec2usec / logging_rate);
    buffer_index    = 0;
    hdf5_file_ptr   = file;
    hdf5_data_type  = HDF5Type<T>::get();
    data_ptr        = data_pointer;
    group           = hdf5_file_ptr->openGroup(group_path);
}

template<typename T, std::size_t buffer_length>
void DatasetOverrides<T, buffer_length>::create_dataset() {
        constexpr std::size_t num_dims = DataTraits<T>::num_dimensions;
        std::array<hsize_t, num_dims> shape = DataTraits<T>::dimension_sizes();

        constexpr std::size_t expanded_dims = num_dims + 1;
        
        std::array<hsize_t, expanded_dims> expanded_size;
        std::array<hsize_t, expanded_dims> max_dimensions;
        std::array<hsize_t, expanded_dims> chunk_size;

        expanded_size[0]  = 0;             
        max_dimensions[0] = H5S_UNLIMITED; 
        chunk_size[0]     = buffer_length;
        
        for (std::size_t i = 0; i < num_dims; i++) {
            expanded_size[i+1]  = shape[i];
            max_dimensions[i+1] = shape[i]; 
            chunk_size[i+1]     = shape[i]; 
        }
        
        property_list.setChunk(expanded_dims, chunk_size.data()); 

        dataspace    = H5::DataSpace(expanded_dims, expanded_size.data(), max_dimensions.data());
        dataset      = group.createDataSet(dataset_name, hdf5_data_type, dataspace, property_list);
        file_space   = dataset.getSpace();
        memory_space = H5::DataSpace(expanded_dims, expanded_size.data());

        old_dataset_size = expanded_size;
        new_dataset_size = expanded_size;
        new_data_shape   = expanded_size;       
}

template<typename T, std::size_t buffer_length>
void DatasetOverrides<T, buffer_length>::log_if_needed(const uint32_t& current_sim_time_usec) {
    if (current_sim_time_usec % logging_dt_usec != 0) {
        return;
    }
    
    // std::cout << "Buffer Index: " << buffer_index <<std::endl;
    
    data_buffer[buffer_index] = *data_ptr;
    buffer_index += 1;
    
    if (buffer_index < buffer_length) {
        return;
    }

    flush_buffer();
    buffer_index = 0;
}

template<typename T, std::size_t buffer_length>
void DatasetOverrides<T, buffer_length>::flush_buffer() {
    
    // std::cout << "Flushing Buffer!\n";
    
    if (buffer_index == 0) {
        return;
    }

    new_dataset_size[0] = old_dataset_size[0] + buffer_index;
    dataset.extend(new_dataset_size.data());

    
    std::array<hsize_t, DataTraits<T>::num_dimensions + 1> start{};
    std::array<hsize_t, DataTraits<T>::num_dimensions + 1> count = new_data_shape;
    
    start[0] = old_dataset_size[0]; // append after the last written entry
    count[0] = buffer_index;        // how many new samples we’re writing

    file_space = dataset.getSpace();
    file_space.selectHyperslab(H5S_SELECT_SET, count.data(), start.data());
    
    memory_space.setExtentSimple(count.size(), count.data());
    
    dataset.write(data_buffer.data(), hdf5_data_type, memory_space, file_space);

    old_dataset_size[0] = new_dataset_size[0];
}

#endif