#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "../../src/math_utilities/math.hpp"
#include "../../external/hdf5/include/H5Cpp.h"

#include <string>
#include <array>
#include <memory>
#include <filesystem>

class HDF5Logger {
public:
  HDF5Logger(const std::string& new_file_name, const std::string& path_to_save_location);

  void create_file();
  void open_file();
  void close_file();

  template<typename T, std::size_t array_size>
  void add_dataset(const std::string& name, const std::array<hsize_t, array_size>& dimensions);
  void add_attibutes(); 
  void add_group(const std::string& new_group_name); 

  bool is_file_open();

  std::string hdf5_file_name;
  std::string full_file_path;

private:  
  void log_data();
  // Maybe add the ability to edit attibutes and groups later?

  void verify_file_path() const;
  void verify_file_exists() const;

  std::unique_ptr<H5::H5File> hdf5_file;
  bool file_is_open; 
  std::string directory_path;

};

#endif