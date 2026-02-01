#ifndef LOGGER_SIM_INTERFACE_HPP
#define LOGGER_SIM_INTERFACE_HPP

#include <memory>
#include <iostream>
#include <string>
#include <format>

#include "../../../sim_framework/sim_includes.hpp"
#include "../../../projects/attitude_filter/sim_files/data_bus.hpp"

class LoggingSimApp : public LoggingAppBase<DataBus> {
public:
    using LoggingAppBase<DataBus>::LoggingAppBase;

    void configure_hdf5_logging(LoggerFacade& logger, const DataBus& data_bus, const AppLoggingRates& rates) override {
        logger.add_group("gyro");

        logger.add_dataset("gyro_meas_valid_flag", "gyro", data_bus.gyro_outputs.measurement_valid,     rates.rate_A_hz);
        logger.add_dataset("meas_delta_angles",    "gyro", data_bus.gyro_outputs.measured_delta_angles, rates.rate_A_hz);
        logger.add_dataset("angle_biases",         "gyro", data_bus.gyro_outputs.angle_biases,          rates.rate_A_hz);
        logger.add_dataset("scale_factors",        "gyro", data_bus.gyro_outputs.scale_factors,         rates.rate_A_hz);
        logger.add_dataset("misalignments",        "gyro", data_bus.gyro_outputs.misalignments,         rates.rate_A_hz);
        logger.add_dataset("seed",                 "gyro", data_bus.gyro_outputs.seed,                  rates.rate_A_hz);
    };
};

#endif