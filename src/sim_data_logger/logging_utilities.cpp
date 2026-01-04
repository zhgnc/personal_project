#include "../../src/sim_data_logger/logging_utilities.hpp"
#include "yaml-cpp/yaml.h"

std::shared_ptr<H5::H5File> LoggingUtilities::create_file(const std::string& full_file_path) {
    // H5File() creates a file if one does not exist or opens the existing file
    // If the file exists, all data is cleared and H5F_ACC_TRUNC gives read and write permissions 
    hdf5_file_ptr = std::make_unique<H5::H5File>(full_file_path, H5F_ACC_TRUNC); 
    file_path     = full_file_path; 
    file_is_open  = true;

    return hdf5_file_ptr;
};

void LoggingUtilities::close_file() {
    LoggingUtilities::verify_file_exists();
  
    if (LoggingUtilities::is_file_open() == false) {  
        return;          
    }

    hdf5_file_ptr->close();
    file_is_open = false;   
};

void LoggingUtilities::open_file() {
    LoggingUtilities::verify_file_exists();

    if (LoggingUtilities::is_file_open() == true) { 
        return;
    }

    hdf5_file_ptr = std::make_unique<H5::H5File>(file_path, H5F_ACC_RDWR);
    file_is_open  = true;
};

bool LoggingUtilities::is_file_open() const { return file_is_open; };

void LoggingUtilities::create_group(const std::string& path_to_group) {
    if (LoggingUtilities::is_file_open() == false) {
        LoggingUtilities::open_file();
        file_is_open = false; 
    }

    std::stringstream remaining_path(path_to_group);
    H5::Group current_group = hdf5_file_ptr->openGroup("/");
    std::string path_segment;

    static constexpr int max_indent   = 100;
    int indent_count = 0;

    while (std::getline(remaining_path, path_segment, '/')) {
        if (path_segment.empty()) { continue; } // Protects against leading '/' and '//' between path segements

        if (current_group.exists(path_segment)) {
            current_group = current_group.openGroup(path_segment);
        } else {
            current_group = current_group.createGroup(path_segment);
        }

        if (indent_count >= max_indent) {
            throw std::runtime_error("[logging_utilities.cpp] Exceeded maximum group nesting depth of " + 
                                      std::to_string(max_indent) + " for this group: " + path_to_group);
        }

        indent_count += 1;
    }
}

void LoggingUtilities::print_file_tree() {
    if (LoggingUtilities::is_file_open() == false) {
        LoggingUtilities::open_file();
        file_is_open = true; 
    }

    H5::Group root_group       = hdf5_file_ptr->openGroup("/");
    std::size_t level_to_print = 0;

    LoggingUtilities::print_file_tree_helper(root_group, level_to_print);
    std::cout << "\n";
};

void LoggingUtilities::print_file_tree_helper(const H5::Group& group, std::size_t level_to_print) {
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

            std::string cpp_data_type = LoggingUtilities::getCppType(hdf5_datatype);

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

// TODO: Make getCppType it's own file like cpp_to_hdf5_type_mapping
std::string LoggingUtilities::getCppType(const H5::DataType& hdf5_type) {    
    struct TypeMapEntry { H5::PredType key; const char* value; };

    static const TypeMapEntry type_map[] = {
        { H5::PredType::NATIVE_INT8,    "int8_t" },
        { H5::PredType::NATIVE_UINT8,   "uint8_t" },
        { H5::PredType::NATIVE_INT16,   "int16_t" },
        { H5::PredType::NATIVE_UINT16,  "uint16_t" },
        { H5::PredType::NATIVE_INT32,   "int32_t" },
        { H5::PredType::NATIVE_UINT32,  "uint32_t" },
        { H5::PredType::NATIVE_INT64,   "int64_t" },
        { H5::PredType::NATIVE_UINT64,  "uint64_t" },
        { H5::PredType::NATIVE_FLOAT,   "float" },
        { H5::PredType::NATIVE_DOUBLE,  "double" },
        { H5::PredType::NATIVE_LDOUBLE, "long double" },
        { H5::PredType::NATIVE_HBOOL,   "bool" },
        { H5::PredType::NATIVE_CHAR,    "char" }
    };

    for (const TypeMapEntry& entry : type_map) {
        if (hdf5_type == entry.key) return entry.value;
    }

    switch (hdf5_type.getClass()) {
        case H5T_STRING: {
            const H5::StrType* string_type = &static_cast<const H5::StrType&>(hdf5_type);
            
            if (string_type != nullptr) {
                if (string_type->isVariableStr()) {
                    return "std::string";
                } else {
                    return "char[256]";
                }
            }

            break;
        }
        case H5T_COMPOUND: return "struct";
        case H5T_ARRAY:    return "array";
        default:           break;
    }

    return "unknown";
}

void LoggingUtilities::verify_file_exists() const {
    if (std::filesystem::exists(file_path) == false) {
        throw std::runtime_error("[logging_utilities.cpp] File does not exist here: " + file_path);
    }
};

void LoggingUtilities::verify_file_path(const std::string& directory_path) const {
    if (std::filesystem::exists(directory_path) == false) {
        throw std::runtime_error("[>logging_utilities.cpp] Path to directory does not exist: " + directory_path);
    }

    if (std::filesystem::is_directory(directory_path) == false) {
        throw std::runtime_error("[logging_utilities.cpp] File path does not lead to a directory: " + directory_path);
    }
};

void LoggingUtilities::verify_group_exists(const std::string& full_group_path) const {
    if (hdf5_file_ptr->exists(full_group_path) == false) {
        throw std::runtime_error("[logging_utilities.cpp] Cannot add data set to a group that does exist. Group path: " + full_group_path);
    }
};