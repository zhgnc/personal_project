#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "../../sim_framework/data_logging/dataset_overrides.hpp"
#include "../../sim_framework/data_logging/dataset_base.hpp"
#include "../../sim_framework/data_logging/hdf5_to_cpp_type_mapping.hpp"
#include "../../sim_framework/data_logging/logger_config.hpp"
#include "../../math//math.hpp"
#include "../../external/hdf5/include/H5Cpp.h"

#include "yaml-cpp/yaml.h"

#include <string>
#include <array>
#include <memory>
#include <filesystem>

class Logger {
public:
    explicit Logger(const std::string& file_path);
    ~Logger();

    // Public functions are only used by logging_app_base.hpp
    void close_file();
    void add_group(const std::string& path_to_group);
    void print_file_tree(const bool& print_file_attributes);
    void log_data(const uint64_t &sim_time_usec);

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
    void create_file(const std::string& full_file_path);
    void open_file(); 
    void print_file_tree_helper(const H5::Group& group, std::size_t level_to_print, const bool& print_file_attributes);
    void print_attributes(const H5::H5Object& object, std::size_t level_to_print);
    void verify_file_exists() const;
    void verify_group_exists(const std::string& full_group_path) const;
    void verify_file_path(const std::string& directory_path) const; // This is unused by maybe useful at some point
    
    template<typename H5ObjType, typename T>
    void write_attribute_to_generic_object(H5ObjType& obj, const std::string& attribute_name, const T& value);

    std::array<std::shared_ptr<DatasetBase>, LoggerConfig::max_dataset_number> datasets;
    std::size_t dataset_count = 0;

    bool file_is_open; 
    std::string file_path;
    std::shared_ptr<H5::H5File> hdf5_file_ptr;

    static constexpr std::size_t buffer_length_config = LoggerConfig::dataset_buffer_length; 

    // Safety gaurds to prevent copying logger instance to another logging class by accident except using std::move
    Logger(const Logger&) = delete;            // Prevents: Logger a("file.h5"); Logger b = a;
    Logger& operator=(const Logger&) = delete; // Prevents: Logger a("a.h5"); Logger b("b.h5"); b = a;
    Logger(Logger&&) = default;                // Allows: Logger a("file.h5"); Logger b = std::move(a);
    Logger& operator=(Logger&&) = default;     // Allows: Logger a("a.h5"); Logger b("b.h5"); b = std::move(a);
};

#include "logger.tpp"  // Template implementations
 

#endif