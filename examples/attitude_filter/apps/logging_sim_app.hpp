#ifndef LOGGER_SIM_INTERFACE_HPP
#define LOGGER_SIM_INTERFACE_HPP

#include <memory>
#include <iostream>
#include <string>
#include <format>

#include "../../../src/sim_framework/logging_app_base.hpp"
#include "../../../src/data_logger/hdf5_logger.hpp"
#include "../../../src/sim_framework/data_bus.hpp"


class LoggingSimApp : public LoggingAppBase {
public:
    using LoggingAppBase::LoggingAppBase;

    void initialize_logger(const int& monte_carlo_run_number) override {
        std::string new_file_name = std::format("test_log_RUN_{:05}.hdf5", monte_carlo_run_number);
        logger = HDF5Logger(new_file_name, data_output_directory);
        logger.create_file();
    };

    void data_to_log () override {
        std::cout << "I'm here 2!\n\n";
    }


private:
    // Need member variables so they persists across step() calls
    DataBus* data_bus;
};

#endif