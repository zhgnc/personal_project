#ifndef ADD_SIM_DATASET_HPP
#define ADD_SIM_DATASET_HPP

#include <string>
#include <array>
#include "../../external/hdf5/include/H5Cpp.h"
#include "../../src/data_logger/cpp_to_hdf5_type_mapping.hpp" 
#include "../../src/data_logger/hdf5_logger.hpp"

template<typename T, std::size_t num_dimensions>
void HDF5Logger::add_sim_dataset(const std::string& dataset_name, const std::array<hsize_t, num_dimensions>& dimensions, const std::string& group_data_is_in) {
    if (is_file_open() == false) {
        open_file();
    }

    if (hdf5_file->exists(group_data_is_in) == false) {
        throw std::runtime_error("[hdf5_logger.cpp] Cannot add data set to a group that does exist. Group path: " + group_data_is_in);
    }

    H5::Group group = hdf5_file->openGroup(group_data_is_in);
    H5::DataType hdf5_datatype = getHDF5Type<T>();

    // Add a dimension for saving data at each simulation step. to first dimension. 
    std::array<hsize_t, num_dimensions + 1> expanded_size;
    std::array<hsize_t, num_dimensions + 1> max_dimensions;
    std::array<hsize_t, num_dimensions + 1> chunk_size;

    expanded_size[0]  = 0;             
    max_dimensions[0] = H5S_UNLIMITED; 
    chunk_size[0]     = 64;
    
    for (size_t i = 0; i < num_dimensions; i++) {
        expanded_size[i+1]  = dimensions[i];
        max_dimensions[i+1] = dimensions[i]; 
        chunk_size[i+1]     = dimensions[i]; 
    }
    
    hsize_t expanded_dimensions = num_dimensions + 1;
    H5::DataSpace num_dimensions_and_sizes(expanded_dimensions, expanded_size.data(), max_dimensions.data());

    H5::DSetCreatPropList property_list;
    property_list.setChunk(expanded_dimensions, chunk_size.data());

    bool if_data_exists_in_group = group.exists(dataset_name);
    if (if_data_exists_in_group == true) {
        group.unlink(dataset_name); // Unlink == delete
    }
    
    group.createDataSet(dataset_name, hdf5_datatype, num_dimensions_and_sizes, property_list);
}

#endif