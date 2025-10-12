#include "hdf5_logger.hpp"

void HDF5Logger::create_file(const std::string& new_file_name, const std::string& path_to_save_location) {
    directory_path = path_to_save_location;
    full_file_path = (std::filesystem::path(directory_path) / new_file_name).string();
    hdf5_file_name = new_file_name;
    
    HDF5Logger::verify_file_path();

    // H5File() creates a file if one does not exist or opens the existing file
    // If the file exists, H5F_ACC_TRUNC gives permission to read & write from the file after clearing all data
    hdf5_file    = std::make_unique<H5::H5File>(full_file_path, H5F_ACC_TRUNC); 
    file_is_open = true;
};

void HDF5Logger::close_file() {  
    if (file_is_open) {  
        hdf5_file->close();
        file_is_open = false;      
    }   
};

void HDF5Logger::open_file(const std::string& new_file_name, const std::string& path_to_save_location) {  
    directory_path = path_to_save_location;
    full_file_path = (std::filesystem::path(path_to_save_location) / new_file_name).string();
    hdf5_file_name = new_file_name;

    HDF5Logger::verify_file_exists();
    
    if (file_is_open) { 
        return;
    }

    hdf5_file    = std::make_unique<H5::H5File>(full_file_path, H5F_ACC_RDWR);
    file_is_open = true;
};

void HDF5Logger::verify_file_path() const {
    if (!std::filesystem::exists(directory_path)) {
        throw std::runtime_error("Save path does not exist: " + directory_path);
    }

    if (!std::filesystem::is_directory(directory_path)) {
        throw std::runtime_error("Save path does not lead to a directory: " + directory_path);
    }
};

void HDF5Logger::verify_file_exists() const {
    if (!std::filesystem::exists(full_file_path)) {
        throw std::runtime_error("File does not exist here: " + full_file_path);
    }
};