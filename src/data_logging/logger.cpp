#include "../../src/data_logging/logger.hpp" 

void Logger::create_file(const std::string& full_file_path) {
    // H5File() creates a file if one does not exist or opens the existing file
    // If the file exists, all data is cleared and H5F_ACC_TRUNC gives read and write permissions 
    hdf5_file_ptr = std::make_unique<H5::H5File>(full_file_path, H5F_ACC_TRUNC); 
    file_path     = full_file_path; 
    file_is_open  = true;
};

void Logger::open_file() {
    verify_file_exists();

    if (file_is_open == true) { 
        return;
    }

    hdf5_file_ptr = std::make_unique<H5::H5File>(file_path, H5F_ACC_RDWR);
    file_is_open  = true;
};

void Logger::close_file() {
    for (const std::unique_ptr<DatasetBase>& dataset : datasets) {
        dataset->flush_buffer();
    }

    verify_file_exists();
  
    if (file_is_open == false) {  
        return;          
    }

    hdf5_file_ptr->close();
    file_is_open = false;
};

void Logger::add_group(const std::string& path_to_group) {
    if (file_is_open == false) {
        open_file();
        file_is_open = true; 
    }

    std::stringstream remaining_path(path_to_group);
    H5::Group current_group = hdf5_file_ptr->openGroup("/");
    std::string path_segment;

    static constexpr int max_indent = 100;
    int indent_count                =   0;

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
};

void Logger::log_data(const uint32_t &sim_time_usec) {
    for (const std::unique_ptr<DatasetBase>& dataset : datasets) {
        dataset->log_if_needed(sim_time_usec);
    }
};

void Logger::print_file_tree() {
    if (file_is_open == false) {
        open_file();
        file_is_open = true; 
    }

    H5::Group root_group       = hdf5_file_ptr->openGroup("/");
    std::size_t level_to_print = 0;

    print_file_tree_helper(root_group, level_to_print);
    std::cout << "\n";
};

void Logger::print_file_tree_helper(const H5::Group& group, std::size_t level_to_print) {
    std::size_t num_objs = group.getNumObjs();
    std::string obj_name;
    H5G_obj_t   obj_type;

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

            std::string cpp_data_type = hdf5_to_cpp_type_mapping(hdf5_datatype);

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

void Logger::verify_file_exists() const {
    if (std::filesystem::exists(file_path) == false) {
        throw std::runtime_error("[logging_utilities.cpp] File does not exist here: " + file_path);
    }
};

void Logger::verify_file_path(const std::string& directory_path) const {
    if (std::filesystem::exists(directory_path) == false) {
        throw std::runtime_error("[>logging_utilities.cpp] Path to directory does not exist: " + directory_path);
    }

    if (std::filesystem::is_directory(directory_path) == false) {
        throw std::runtime_error("[logging_utilities.cpp] File path does not lead to a directory: " + directory_path);
    }
};

void Logger::verify_group_exists(const std::string& full_group_path) const {
    if (hdf5_file_ptr->exists(full_group_path) == false) {
        throw std::runtime_error("[logging_utilities.cpp] Cannot add data set to a group that does exist. Group path: " + full_group_path);
    }
};