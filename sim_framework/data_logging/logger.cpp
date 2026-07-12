#include "../../sim_framework/data_logging/logger.hpp"

Logger::Logger(const std::string& path_to_file) {
    std::lock_guard<std::recursive_mutex> lock(hdf5_mutex);
    
    file_path = path_to_file;
    create_file(file_path);
}

Logger::~Logger() {
    std::lock_guard<std::recursive_mutex> lock(hdf5_mutex);

    close_file();

    // In C++ a class' data members are only destroyed after the destructor 
    // finishes executing. When running in parallel `datasets` and `hdf5_file_ptr`
    // cause race conditions that are not prevented by the `hdf5_mutex` because
    // it goes out of scope before those members are deleted. For this reason 
    // those two members must be manually reset 
    for (std::size_t i = 0; i < dataset_count; i++) {
        datasets[i].reset();
    }

    hdf5_file_ptr.reset();
}

void Logger::create_file(const std::string& full_file_path) {
    // H5File() creates a file if one does not exist or opens the existing file
    // If the file exists, all data is cleared and H5F_ACC_TRUNC gives read and write permissions
    std::lock_guard<std::recursive_mutex> lock(hdf5_mutex);

    hdf5_file_ptr = std::make_unique<H5::H5File>(full_file_path, H5F_ACC_TRUNC);
    file_path     = full_file_path; 
    file_is_open  = true;
}

void Logger::open_file() {
    std::lock_guard<std::recursive_mutex> lock(hdf5_mutex);

    verify_file_exists();

    if (file_is_open == true) { 
        return;
    }

    hdf5_file_ptr = std::make_unique<H5::H5File>(file_path, H5F_ACC_RDWR);
    file_is_open  = true;
}

void Logger::close_file() {
    std::lock_guard<std::recursive_mutex> lock(hdf5_mutex);
 
    if (file_is_open == false) {  
        return;          
    }
    
    for (std::size_t i = 0; i < dataset_count; ++i) {
        datasets[i]->flush_buffer();
    }

    verify_file_exists();

    hdf5_file_ptr->close();
    file_is_open = false;
}

void Logger::add_group(const std::string& path_to_group) {
    std::lock_guard<std::recursive_mutex> lock(hdf5_mutex);
 
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
            throw std::runtime_error("[logger.cpp] Exceeded maximum group nesting depth of " + 
                                      std::to_string(max_indent) + " for this group: " + path_to_group);
        }

        indent_count += 1;
    }
}

// Intentionally NOT locked: log_if_needed() only copies data into this Logger's own
// RAM buffers, which no other thread touches, so taking the global hdf5_mutex here
// would stall every sim thread's step loop whenever one thread is doing file I/O.
// When a buffer fills, flush_buffer() takes the lock itself for the actual HDF5 write.
void Logger::log_data(const uint64_t &sim_time_usec) {
    for (std::size_t i = 0; i < dataset_count; i++) {
        datasets[i]->log_if_needed(sim_time_usec);
    }
}

void Logger::print_file_tree(const bool& print_file_attributes) {
    std::lock_guard<std::recursive_mutex> lock(hdf5_mutex);
 
    if (file_is_open == false) {
        open_file();
        file_is_open = true; 
    }

    // Must close file so that each dataset buffers is flushed otherwise the hdf5 
    // file will not know how much data is in each dataset when printing the size
    close_file();
    open_file();

    H5::Group root_group       = hdf5_file_ptr->openGroup("/");
    std::size_t level_to_print = 0;

    print_file_tree_helper(root_group, level_to_print, print_file_attributes);
    std::cout << "\n";
}

void Logger::print_file_tree_helper(const H5::Group& group, 
                                    std::size_t level_to_print, 
                                    const bool& print_file_attributes) 
{
    std::lock_guard<std::recursive_mutex> lock(hdf5_mutex);

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
            std::cout << "|- " << "\033[1;36m" << obj_name << " (Group)\n" << "\033[0m"; // Blue

            H5::Group subgroup = group.openGroup(obj_name);

            if (print_file_attributes == true) {
                print_attributes(subgroup, level_to_print + 1);
            }

            print_file_tree_helper(subgroup, level_to_print + 1, print_file_attributes);

        } else if (obj_type == H5G_DATASET) {
            std::cout << "\033[32m" << "|- " << obj_name << " (Dataset) " << "\033[0m"; // Green
            
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

            if (print_file_attributes == true) {
                print_attributes(dataset, level_to_print + 1);
            }

        } else {
            std::cout << " (Other)\n";
        }
    }
}

void Logger::print_attributes(const H5::H5Object& object, std::size_t level_to_print) {
    std::lock_guard<std::recursive_mutex> lock(hdf5_mutex);
 
    const int num_attrs = object.getNumAttrs();

    for (int i = 0; i < num_attrs; i++) {
        H5::Attribute attr = object.openAttribute(i);

        std::string attr_name = attr.getName();
        H5::DataType type     = attr.getDataType();
        H5::DataSpace space   = attr.getSpace();

        std::string cpp_type = hdf5_to_cpp_type_mapping(type);

        for (std::size_t j = 0; j < level_to_print; j++) {
            std::cout << "    ";
        }

        std::cout << "\033[34m|- " << attr_name << " (Attribute)\033[0m {Type: " << cpp_type << "}\n";
    }
}

// Not locked: only touches std::filesystem, which is thread-safe, never the HDF5 API
void Logger::verify_file_exists() const {
    if (std::filesystem::exists(file_path) == false) {
        throw std::runtime_error("[logger.cpp] File does not exist here: " + file_path);
    }
}

// Not locked: only touches std::filesystem, which is thread-safe, never the HDF5 API
void Logger::verify_file_path(const std::string& directory_path) const {
    if (std::filesystem::exists(directory_path) == false) {
        throw std::runtime_error("[>logger.cpp] Path to directory does not exist: " + directory_path);
    }

    if (std::filesystem::is_directory(directory_path) == false) {
        throw std::runtime_error("[logger.cpp] File path does not lead to a directory: " + directory_path);
    }
}

void Logger::verify_group_exists(const std::string& full_group_path) const {
    std::lock_guard<std::recursive_mutex> lock(hdf5_mutex);

    if (hdf5_file_ptr->exists(full_group_path) == false) {
        throw std::runtime_error("[logger.cpp] Cannot add data set to a group that does exist. Group path: " + full_group_path);
    }
}