#ifndef LOGGER_SIM_INTERFACE_HPP
#define LOGGER_SIM_INTERFACE_HPP

#include <memory>
#include <iostream>
#include <string>

#include "../../../src/sim_framework/logging_app_base.hpp"
#include "../../../src/data_logger/hdf5_logger.hpp"
#include "../../../src/sim_framework/data_bus.hpp"


class LoggingSimApp : public LoggingAppBase {
public:
    using LoggingAppBase::LoggingAppBase;

    void initialize_logger(const std::string& file_name) {
        logger = HDF5Logger(file_name, data_output_directory);

        std::cout << "I'm here 1! " << file_name << "\n\n";
    };

    void data_to_log () override {
        std::cout << "I'm here 2!\n\n";
    }


private:
    // Need member variables so they persists across step() calls
    HDF5Logger logger; 
    DataBus* data_bus;
};

#endif