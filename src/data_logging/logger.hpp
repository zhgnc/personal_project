#ifndef logger_HPP
#define logger_HPP

#include "../../src/data_logging/dataset_overrides.hpp"
#include "../../src/data_logging/dataset_base.hpp"
#include "../../src/data_logging/hdf5_to_cpp_type_mapping.hpp"
#include "../../src/math/math.hpp"
#include "../../external/hdf5/include/H5Cpp.h"

#include "yaml-cpp/yaml.h"

#include <string>
#include <array>
#include <memory>
#include <filesystem>
#include <vector>

class Logger {
public:
    Logger() = default;

    // Public functions are only used by logging_app_base.hpp
    void create_file(const std::string& full_file_path);
    void close_file();
    void add_group(const std::string& path_to_group);
    void print_file_tree();
    void log_data(const uint32_t &sim_time_usec);

    template<typename T>
    void add_dataset(const std::string& dataset_name, 
                     const std::string& full_group_path, 
                     const T& data_reference, 
                     const double record_rate_hz);
    
    template<typename T>
    void write_attribute(const std::string& group_path,
                         const std::string& attribute_name,
                         const T& value);

private: 
    void open_file(); 
    void print_file_tree_helper(const H5::Group& group, std::size_t level_to_print);
    void print_attributes(const H5::H5Object& object, std::size_t level_to_print);
    void verify_file_exists() const;
    void verify_group_exists(const std::string& full_group_path) const;
    void verify_file_path(const std::string& directory_path) const; // This is unused by maybe useful at some point
    
    template<typename H5ObjType, typename T>
    void write_attribute_to_generic_object(H5ObjType& obj, const std::string& attribute_name, const T& value);

    std::shared_ptr<H5::H5File> hdf5_file_ptr;
    std::vector<std::unique_ptr<DatasetBase>> datasets;
    bool file_is_open; 
    std::string file_path;
    static constexpr std::size_t buffer_length_config = 1000;
};

#include "logger.tpp"  // Template implementations
 

#endif