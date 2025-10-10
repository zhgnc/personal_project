#ifndef GYRO_SIM_APP_HPP
#define GYRO_SIM_APP_HPP

#include <memory>
#include <iostream>
#include <string>

#include "../../../src/sim_framework/sim_app_base_interface.hpp"
#include "../../../src/sensor_models/gyro/gyro_model.hpp"


class GyroSimInterface : public SimAppBaseInterface {
public:
    GyroSimInterface() = default;

    void initialize(const std::string& path_to_config) override {
        gyro = GyroModel(path_to_config);
        gyro.initialize();
    }

    void step() override {
        gyro.inputs.q_j2000_to_body_true = {0.5, 0.5, 0.5, 0.5};
        gyro.copy_inputs_to_class();
        gyro.execute();
        gyro.set_outputs();

        std::cout << gyro.outputs.gyro_measurement_valid << "\n";
        gyro.outputs.measured_delta_angles.print();
    }


private:
    GyroModel gyro; // Need member variable gyro so it persists across step() calls 
};

#endif