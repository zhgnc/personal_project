#ifndef LOGGING_FACADE_HPP
#define LOGGING_FACADE_HPP


#include <string>
#include <typeinfo>

#include "../data_logging/logger.hpp"

// This class exists so that when users are configuring the HDF5 file in their logging app 
// classes they only have access to the `add_group` and `add_dataset` portions of the logging
// class and not function like `create_file` or `log_data`
class LoggerFacade {
public:
    explicit LoggerFacade(Logger& logger) : wrapped_logger(logger) {}

    void add_group(const std::string& group_path) {
        wrapped_logger.add_group(group_path);
    }

    template<typename T>
    void add_dataset(const std::string& dataset_name, 
                     const std::string& group_path, 
                     const T& data_reference, 
                     const double record_rate_hz) 
    {
        wrapped_logger.add_dataset(dataset_name, group_path, data_reference, record_rate_hz);
    }

    template<typename T>
    void write_attribute(const std::string& group_path, const std::string& attribute_name, const T& value) {
        wrapped_logger.write_attribute(group_path, attribute_name, value);
    }

private:
    Logger& wrapped_logger;
};

#endif
