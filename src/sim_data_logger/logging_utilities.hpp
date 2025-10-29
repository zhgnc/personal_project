#ifndef LOGGING_UTILITIES_HPP
#define LOGGING_UTILITIES_HPP

#include "../../src/math_utilities/math.hpp"
#include "../../external/hdf5/include/H5Cpp.h"

#include <string>
#include <array>
#include <memory>
#include <filesystem>

class LoggingUtilities {
public:
  LoggingUtilities() = default;

  std::shared_ptr<H5::H5File> create_file(const std::string& full_file_path);
  void open_file();
  void close_file();
  bool is_file_open() const;
  void create_group(const std::string& path_to_group);
  void print_file_tree();
  void verify_file_path(const std::string& directory_path) const;
  void verify_file_exists() const;

private:  
  void print_file_tree_helper(const H5::Group& group, std::size_t level_to_print);
  std::string getCppType(const H5::DataType& hdf5_type);


  std::shared_ptr<H5::H5File> hdf5_file_ptr;
  bool file_is_open; 
  std::string file_path;  
};

// // #include templated header implementations after class definition for proper linking
// #include "../../src/data_logger/add_dataset.hpp"
// #include "../../src/data_logger/add_sim_dataset.hpp"
// #include "../../src/data_logger/data_read_write_utilities.hpp"

#endif