#ifndef LOGGER_SIM_INTERFACE_HPP
#define LOGGER_SIM_INTERFACE_HPP

#include <memory>
#include <iostream>
#include <string>
#include <format>

#include "../../../src/sim_framework/logging_app_base.hpp"
#include "../../../src/sim_framework/data_bus.hpp"


class LoggingSimApp : public LoggingAppBase {
public:
    using LoggingAppBase::LoggingAppBase;

    void configure_hdf5_file() override {
        logger.add_group("group_1/group_3");
        logger.add_group("group_1/group_4");
        logger.add_group("group_2");
        logger.add_group("gyro");

        int record_freq_hz = 1; // TODO there is a bug when this is not 1

        logger.add_dataset<bool>("Gyro_Meas_Valid", "gyro", data_bus->gyro_outputs.measurement_valid, record_freq_hz);
        // logger.add_dataset<matrix<double,2,2>>("Test_Data_Point_2", "group_1/group_3", std::make_shared<matrix<double,2,2>>(M1), record_freq_hz);
        logger.add_dataset<vector<double,3>>("Delta_Angles", "gyro", data_bus->gyro_outputs.measured_delta_angles, record_freq_hz);
        logger.add_dataset<int>("Test_Increment", "gyro", data_bus->gyro_outputs.test_increment, record_freq_hz);
        logger.print_file_tree();
    };

private:
    // Need member variables so they persists across step() calls
    // std::size_t data_step = 0; 
};

#endif