#ifndef READ_HDF5_DATA_HPP
#define READ_HDF5_DATA_HPP

#include "../../external/hdf5/include/H5Cpp.h"
#include "../../src/data_logging/cpp_to_hdf5_type_mapping.hpp"

#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

template<typename T>
std::vector<T> read_hdf5_dataset(const std::string& hdf5_file_path, const std::string& full_dataset_path) {
    // This function is intended for unit tests only so returning std::vector is lower risk
    
    // Verify file exists (copied from logger.cpp)
    if (std::filesystem::exists(hdf5_file_path) == false) {
        throw std::runtime_error("[read_hdf5_data.hpp] File does not exist here: " + hdf5_file_path);
    }

    // Open file in read-only mode
    H5::H5File file(hdf5_file_path, H5F_ACC_RDONLY);
    
    // Verify dataset exists
    if (file.exists(full_dataset_path) == false) {
        throw std::runtime_error("[read_hdf5_data.hpp] Dataset does not exist: " + full_dataset_path);
    }

    // Open dataset
    H5::DataSet dataset = file.openDataSet(full_dataset_path);
    H5::DataSpace space = dataset.getSpace();
    H5::DataType type   = dataset.getDataType();

    // Verify datatype matches template type 'T'
    // Note, htri_t H5Tequal(hid_t type1_id, hid_t type2_id); and if it returns 0 then the types do not match
    H5::DataType expected_type = HDF5Type<T>::get();
    if (H5Tequal(type.getId(), expected_type.getId()) == 0) {
        throw std::runtime_error("[read_hdf5_data.hpp] Datatype mismatch for dataset: " + full_dataset_path);
    }

    // Determine dataset size
    int num_dimensions = space.getSimpleExtentNdims();
    std::vector<hsize_t> dimensions(num_dimensions);
    space.getSimpleExtentDims(dimensions.data());

    hsize_t total_elements = 1;
    for (hsize_t dimension_length : dimensions) {
        total_elements *= dimension_length;
    }

    std::vector<T> data(total_elements);

    // Read entire dataset
    dataset.read(data.data(), expected_type);

    // The file is closed when it goes out of scope but explicitly closing the file make it easier to recall
    file.close();

    return data;
}

// Generic function to read an attribute from any HDF5 object (dataset or group)
template<typename H5ObjType, typename T>
T read_attribute_from_generic_object(H5ObjType& obj, const std::string& attribute_name) {
    if (obj.attrExists(attribute_name) == false) {
        throw std::runtime_error("[read_hdf5_data.hpp] Attribute does not exist: " + attribute_name);
    }

    H5::Attribute attr = obj.openAttribute(attribute_name);
    H5::DataType datatype = HDF5Type<T>::get();

    // Verify type matches
    if (H5Tequal(attr.getDataType().getId(), datatype.getId()) == 0) {
        throw std::runtime_error("[read_hdf5_data.hpp] Attribute type mismatch: " + attribute_name);
    }

    T value{};
    if constexpr (std::is_same_v<T,std::string>) {
        attr.read(datatype, value);
    } else {
        attr.read(datatype, &value);
    }

    return value;
}

template<typename T>
T read_hdf5_attribute(const std::string& hdf5_file_path, const std::string& object_path, const std::string& attribute_name) {
    if (std::filesystem::exists(hdf5_file_path) == false) {
        throw std::runtime_error("[read_hdf5_data.hpp] File does not exist here: " + hdf5_file_path);
    }

    H5::H5File file(hdf5_file_path, H5F_ACC_RDONLY);
    
    if (file.nameExists(object_path) == false) {
        throw std::runtime_error("[read_hdf5_data.hpp] HDF5 object does not exist: " + object_path);
    }

    // Get object info
    H5O_info2_t object_info{};
    unsigned fields = H5O_INFO_BASIC;
    H5Oget_info_by_name3(file.getId(), object_path.c_str(), &object_info, fields, H5P_DEFAULT);

    // Read attribute depending on object type
    T attribute{};
    if (object_info.type == H5O_TYPE_GROUP) {
        H5::Group group = file.openGroup(object_path);
        attribute = read_attribute_from_generic_object<H5::Group, T>(group, attribute_name);

    } else if (object_info.type == H5O_TYPE_DATASET) {
        H5::DataSet dataset = file.openDataSet(object_path);
        attribute = read_attribute_from_generic_object<H5::DataSet, T>(dataset, attribute_name);

    } else {
        throw std::runtime_error("[read_hdf5_data.hpp] Unsupported HDF5 object type: " + object_path);
    }

    file.close();

    return attribute;
}

#endif
