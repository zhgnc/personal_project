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

    void configure_hdf5_file() override {
        // std::cout << "I'm here 1!\n\n";
    };

    void data_to_log () override {
        // std::cout << "I'm here 2!\n\n";
    };


private:
    // Need member variables so they persists across step() calls
    DataBus* data_bus;
};

#endif