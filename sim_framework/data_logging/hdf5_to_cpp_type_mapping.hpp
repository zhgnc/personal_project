#ifndef HDF5_TO_CPP_TYPE_MAPPING_HPP
#define HDF5_TO_CPP_TYPE_MAPPING_HPP

#include <string>
#include <H5Cpp.h>

std::string hdf5_to_cpp_type_mapping(const H5::DataType& hdf5_type);

#endif