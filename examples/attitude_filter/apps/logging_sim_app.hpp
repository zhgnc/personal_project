#ifndef LOGGER_SIM_INTERFACE_HPP
#define LOGGER_SIM_INTERFACE_HPP

#include <memory>
#include <iostream>
#include <string>
#include <format>

#include "../../../src/sim_framework/logging_app_base.hpp"
#include "../../../examples/attitude_filter/apps/data_bus.hpp"

class LoggingSimApp : public LoggingAppBase<DataBus> {
public:
    using LoggingAppBase<DataBus>::LoggingAppBase;

    void config_hdf5_with_app_data(Logger& logger, DataBus& data_bus, AppLoggingRates& rates) override {
        logger.add_group("gyro");

        logger.add_dataset<bool>("Gyro_Meas_Valid",          "gyro", data_bus.gyro_outputs.measurement_valid,     rates.rate_A_hz);
        logger.add_dataset<vector<double,3>>("Delta_Angles", "gyro", data_bus.gyro_outputs.measured_delta_angles, rates.rate_A_hz);
    };
};

#endif