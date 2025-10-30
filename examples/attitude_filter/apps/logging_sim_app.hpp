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
        logger.print_file_tree();

        int record_freq_hz = 1;

        logger.add_dataset<bool>("Test_Data_Point_1", "group_1", std::make_shared<bool>(data_bus->gyro_outputs.measurement_valid), record_freq_hz);
        // logger.add_dataset<matrix<double,2,2>>("Test_Data_Point_2", "group_1/group_3", std::make_shared<matrix<double,2,2>>(M1), record_freq_hz);
        logger.add_dataset<vector<double,3>>("Test_Data_Point_3", "group_1/group_4", std::make_shared<vector<double,3>>(data_bus->gyro_outputs.measured_delta_angles), record_freq_hz);
    };

    void data_to_log () override {
        // std::cout << "I'm Here 2!\n";
        // double temp_array[3];
        
        // // std::cout << "Data appended to hdf5 file:\n";
        // for (int i = 0; i < 3; ++i) {
        //     temp_array[i] = data_bus->gyro_outputs.measured_delta_angles(i);
            
        //     // std::cout << temp_array[i] << ", ";
        // }
        // // std::cout << "\n";

        // // std::cout << "Data bus output in logging sim app:\n";
        // // data_bus->gyro_outputs.measured_delta_angles.print(); 

        // logger.append_data<double>("gyro/delta_thetas", {1,3}, temp_array); 

        // // std::unique_ptr<double[]> data_buffer_out(new double[3]);
        // // logger.read_data<double>("gyro/delta_thetas", data_buffer_out.get(), {data_step,0}, {1,3});

        // // std::cout << "Data retrieved from hdf5 file:\n";
        // // for (int i = 0; i < 3; ++i) {
        // //     std::cout << data_buffer_out[i] << "    ";
        // // }
        // // std::cout << "\n\n";

        // // data_step += 1;
    };


private:
    // Need member variables so they persists across step() calls
    // std::size_t data_step = 0; 
};

#endif