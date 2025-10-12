#include "hdf5_logger.hpp"

HDF5Logger::HDF5Logger(const std::string& new_file_name, const std::string& path_to_save_location) {
    directory_path = path_to_save_location;
    full_file_path = (std::filesystem::path(directory_path) / new_file_name).string();
    hdf5_file_name = new_file_name;
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
  
    if (file_is_open) {  
        hdf5_file->close();
        file_is_open = false;      
    }   
};

void HDF5Logger::open_file() {
    HDF5Logger::verify_file_exists();

    if (file_is_open) { 
        return;
    }

    hdf5_file    = std::make_unique<H5::H5File>(full_file_path, H5F_ACC_RDWR);
    file_is_open = true;
};

bool HDF5Logger::is_file_open() {
    return file_is_open;
};

void HDF5Logger::verify_file_path() const {
    if (!std::filesystem::exists(directory_path)) {
        throw std::runtime_error("[hdf5_logger.cpp] Path to directory does not exist: " + directory_path);
    }

    if (!std::filesystem::is_directory(directory_path)) {
        throw std::runtime_error("[hdf5_logger.cpp] File path does not lead to a directory: " + directory_path);
    }
};

void HDF5Logger::verify_file_exists() const {
    if (!std::filesystem::exists(full_file_path)) {
        throw std::runtime_error("[hdf5_logger.cpp] File does not exist (wrong name, bad path, or file does not exist): " + full_file_path);
    }
};