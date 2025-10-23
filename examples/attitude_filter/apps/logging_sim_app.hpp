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
        logger.add_group("fake_dynamics");
        logger.add_group("gyro");
        logger.add_group("star_tracker");
        logger.add_group("attitude_filter");

        logger.add_sim_dataset<double>("fake_quaternion", {3}, "fake_dynamics");

        logger.add_sim_dataset<double>("delta_thetas", {3}, "gyro");
        logger.add_sim_dataset<bool>("valid_measurement", {1}, "gyro");

        logger.print_file_tree();
    };

    void data_to_log () override {
        double temp_array[3];
        
        std::cout << "Data appended to hdf5 file:\n";
        for (int i = 0; i < 3; ++i) {
            temp_array[i] = data_bus->gyro_outputs.measured_delta_angles(i);
            
            std::cout << temp_array[i] << ", ";
        }
        std::cout << "\n";

        // std::cout << "Data bus output in logging sim app:\n";
        // data_bus->gyro_outputs.measured_delta_angles.print(); 

        logger.append_data<double>("gyro/delta_thetas", {1,3}, temp_array);

        std::unique_ptr<double[]> data_buffer_out(new double[3]);
        logger.read_data<double>("gyro/delta_thetas", data_buffer_out.get(), {data_step,0}, {1,3});

        std::cout << "Data retrieved from hdf5 file:\n";
        for (int i = 0; i < 3; ++i) {
            std::cout << data_buffer_out[i] << "    ";
        }
        std::cout << "\n\n";

        data_step += 1;
    };


private:
    // Need member variables so they persists across step() calls
    std::size_t data_step = 0; 
};

#endif