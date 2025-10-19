#ifndef LOGGER_SIM_INTERFACE_HPP
#define LOGGER_SIM_INTERFACE_HPP

#include <memory>
#include <iostream>
#include <string>

#include "../../../src/sim_framework/sim_app_base_interface.hpp"
#include "../../../src/data_logger/hdf5_logger.hpp"
#include "../../../src/sim_framework/data_bus.hpp"


class LoggerSimInterface : public SimAppBaseInterface {
public:
    LoggerSimInterface() = default;

    void initialize(const std::string& path_to_config, DataBus& bus) override {
        logger   = HDF5Logger(path_to_config);
        data_bus = &bus;
    }

    void step() override {
        std::cout << "I'm here!\n\n";
    }


private:
    // Need member variables so they persists across step() calls
    HDF5Logger logger; 
    DataBus* data_bus;
};

#endif