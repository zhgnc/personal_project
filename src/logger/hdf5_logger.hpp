#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "../../src/math_utilities/math.hpp"
#include "../../external/hdf5/include/H5Cpp.h"

#include <string>
#include <array>
#include <memory>
#include <filesystem>
#include <iostream>
#include <vector>

class HDF5Logger {
public:
  HDF5Logger(const std::string& new_file_name, const std::string& path_to_save_location);

  void create_file();
  void open_file();
  void close_file();

  template<typename T, std::size_t num_dimensions>
  void add_dataset(const std::string& name, const std::array<hsize_t, num_dimensions>& dimensions, const std::string& group_data_is_in);
  template<typename T, std::size_t num_dimensions>
  void add_sim_dataset(const std::string& name, const std::array<hsize_t, num_dimensions>& dimensions, const std::string& group_data_is_in);
  void add_attibutes(); // Create this
  void add_group(const std::string& new_group_name); 

  void log_data(); // Create this
  void print_file_tree();
  bool is_file_open();

  std::string hdf5_file_name;
  std::string full_file_path;

private:
  void verify_file_path() const;
  void verify_file_exists() const;
  void print_file_tree_helper(const H5::Group& group, std::size_t level_to_print);
  std::string getCppType(const H5::DataType& hdf5_type);

  std::unique_ptr<H5::H5File> hdf5_file;
  bool file_is_open; 
  std::string directory_path;

};

// #include templated header implementations after class definition for proper linking
#include "../../src/logger/add_dataset.hpp"
#include "../../src/logger/add_sim_dataset.hpp"

#endif