#include "../../src/data_logger/hdf5_logger.hpp"
#include "yaml-cpp/yaml.h"

HDF5Logger::HDF5Logger(const std::string& file_name, const std::string& path_to_file) {
    full_file_path = (std::filesystem::path(path_to_file) / file_name).string();
    file_is_open   = false;

    HDF5Logger::verify_file_path();
};

void HDF5Logger::create_file() {
    // H5File() creates a file if one does not exist or opens the existing file
    // If the file exists, H5F_ACC_TRUNC gives permission to read & write from the file after clearing all data
    hdf5_file    = std::make_unique<H5::H5File>(full_file_path, H5F_ACC_TRUNC); 
    file_is_open = true;
};

void HDF5Logger::close_file() {
    HDF5Logger::verify_file_exists();
  
    if (HDF5Logger::is_file_open() == true) {  
        hdf5_file->close();
        file_is_open = false;      
    }   
};

void HDF5Logger::open_file() {
    HDF5Logger::verify_file_exists();

    if (HDF5Logger::is_file_open() == true) { 
        return;
    }

    hdf5_file    = std::make_unique<H5::H5File>(full_file_path, H5F_ACC_RDWR);
    file_is_open = true;
};

bool HDF5Logger::is_file_open() {
    return file_is_open;
};

void HDF5Logger::verify_file_path() const {
    if (std::filesystem::exists(directory_path) == false) {
        throw std::runtime_error("[hdf5_logger.cpp] Path to directory does not exist: " + directory_path);
    }

    if (std::filesystem::is_directory(directory_path) == false) {
        throw std::runtime_error("[hdf5_logger.cpp] File path does not lead to a directory: " + directory_path);
    }
};

void HDF5Logger::verify_file_exists() const {
    if (std::filesystem::exists(full_file_path) == false) {
        throw std::runtime_error("[hdf5_logger.cpp] File does not exist here: " + full_file_path);
    }
};

void HDF5Logger::add_group(const std::string& full_path_to_new_group) {
    if (HDF5Logger::is_file_open() == false) {
        HDF5Logger::open_file();
    }

    std::stringstream remaining_path(full_path_to_new_group);
    H5::Group current_group = hdf5_file->openGroup("/");
    std::string path_segment;

    while (std::getline(remaining_path, path_segment, '/')) {
        if (path_segment.empty()) { continue; } // Protects against leading '/' and '//' between path segements

        if (current_group.exists(path_segment)) {
            current_group = current_group.openGroup(path_segment);
        } else {
            current_group = current_group.createGroup(path_segment);
        }
    }
}

void HDF5Logger::print_file_tree() {
    if (HDF5Logger::is_file_open() == false) {
        HDF5Logger::open_file();
    }

    H5::Group root_group       = hdf5_file->openGroup("/");
    std::size_t level_to_print = 0;

    HDF5Logger::print_file_tree_helper(root_group, level_to_print);
    std::cout << "\n";
}

void HDF5Logger::print_file_tree_helper(const H5::Group& group, std::size_t level_to_print) {
    std::size_t num_objs = group.getNumObjs();
    std::string obj_name;
    H5G_obj_t obj_type;

    for (std::size_t i = 0; i < num_objs; i++) {
        obj_name = group.getObjnameByIdx(i);
        obj_type = group.getObjTypeByIdx(i);

        for (std::size_t j = 0; j < level_to_print; j++) {
            std::cout << "    ";
        }

        if (obj_type == H5G_GROUP) {
            std::cout << "|- " << "\033[34m" << obj_name << " (Group)\n" << "\033[0m";

            H5::Group subgroup = group.openGroup(obj_name);
            print_file_tree_helper(subgroup, level_to_print + 1);

        } else if (obj_type == H5G_DATASET) {
            std::cout << "\033[32m" << "|- " << obj_name << " (Dataset) " << "\033[0m";
            
            H5::DataSet dataset        = group.openDataSet(obj_name);
            H5::DataType hdf5_datatype = dataset.getDataType();
            H5::DataSpace dataspace    = dataset.getSpace();

            std::string cpp_data_type = HDF5Logger::getCppType(hdf5_datatype);

            int num_dimensions = dataspace.getSimpleExtentNdims();
            std::vector<hsize_t> dataset_dimensions(num_dimensions);
            dataspace.getSimpleExtentDims(dataset_dimensions.data());

            std::cout << "{Type: " << cpp_data_type << ", Shape: [";
            
            for (std::size_t dim_num = 0; dim_num < dataset_dimensions.size(); dim_num++) {
                std::cout << dataset_dimensions[dim_num];
                bool if_not_last_element = dim_num < dataset_dimensions.size() - 1;

                if (if_not_last_element) {
                    std::cout << ",";
                }
            }

            std::cout << "]}\n";

        } else {
            std::cout << " (Other)\n";
        }
    }
}

std::string HDF5Logger::getCppType(const H5::DataType& hdf5_type) {
    if (hdf5_type == H5::PredType::NATIVE_INT8)    return "int8_t";
    if (hdf5_type == H5::PredType::NATIVE_UINT8)   return "uint8_t";
    if (hdf5_type == H5::PredType::NATIVE_INT16)   return "int16_t";
    if (hdf5_type == H5::PredType::NATIVE_UINT16)  return "uint16_t";
    if (hdf5_type == H5::PredType::NATIVE_INT32)   return "int32_t";
    if (hdf5_type == H5::PredType::NATIVE_UINT32)  return "uint32_t";
    if (hdf5_type == H5::PredType::NATIVE_INT64)   return "int64_t";
    if (hdf5_type == H5::PredType::NATIVE_UINT64)  return "uint64_t";

    if (hdf5_type == H5::PredType::NATIVE_FLOAT)   return "float";
    if (hdf5_type == H5::PredType::NATIVE_DOUBLE)  return "double";
    if (hdf5_type == H5::PredType::NATIVE_LDOUBLE) return "long double";
 
    if (hdf5_type == H5::PredType::NATIVE_HBOOL)   return "bool";
    if (hdf5_type == H5::PredType::NATIVE_CHAR)    return "char";

    if (hdf5_type.getClass() == H5T_STRING) {
        const H5::StrType& strType = static_cast<const H5::StrType&>(hdf5_type);
        
        if (strType.isVariableStr()) {
            return "std::string"; // Variable length string 
        } else {
            return "char[256]"; // Fixed length string 
        }
    }

    return "unknown";
}

std::vector<hsize_t> HDF5Logger::get_dataset_dimensions(const H5::DataSet& dataset) {
    H5::DataSpace dataspace  = dataset.getSpace();
    int number_of_dimensions = dataspace.getSimpleExtentNdims();

    std::vector<hsize_t> dataset_dimensions(number_of_dimensions);
    dataspace.getSimpleExtentDims(dataset_dimensions.data(), nullptr);

    return dataset_dimensions;
};