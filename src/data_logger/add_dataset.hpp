#ifndef ADD_DATASET_HPP
#define ADD_DATASET_HPP

#include <string>
#include <array>
#include "../../external/hdf5/include/H5Cpp.h"
#include "../../src/data_logger/cpp_to_hdf5_type_mapping.hpp" 
#include "../../src/data_logger/hdf5_logger.hpp"

template<typename T>
void HDF5Logger::add_dataset(const std::string& dataset_name, const std::initializer_list<hsize_t>& dimension_vector, const std::string& group_data_is_in) {
    if (is_file_open() == false) {
        open_file();
    }

    if (hdf5_file->exists(group_data_is_in) == false) {
        throw std::runtime_error("[hdf5_logger.cpp] Cannot add data set to a group that does exist. Group path: " + group_data_is_in);
    }

    H5::Group group = hdf5_file->openGroup(group_data_is_in);
    
    std::vector<hsize_t> dimensions(dimension_vector);
    hsize_t num_dimensions = dimensions.size(); 

    H5::DataType hdf5_datatype = getHDF5Type<T>();
    H5::DataSpace num_dimensions_and_sizes(num_dimensions, dimensions.data());

    if (group.exists(dataset_name)) {
        group.unlink(dataset_name); // Unlink == delete
    }
    
    group.createDataSet(dataset_name, hdf5_datatype, num_dimensions_and_sizes);
}

#endif