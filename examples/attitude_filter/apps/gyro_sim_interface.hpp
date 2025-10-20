#ifndef GYRO_SIM_APP_HPP
#define GYRO_SIM_APP_HPP

#include <memory>
#include <iostream>
#include <string>

#include "../../../src/sim_framework/sim_app_base_interface.hpp"
#include "../../../src/sensor_models/gyro/gyro_model.hpp"
#include "../../../src/sim_framework/data_bus.hpp"


class GyroSimInterface : public SimAppBaseInterface {
public:
    GyroSimInterface() = default;

    void initialize(const std::string& path_to_config, DataBus& bus) override {
        gyro = GyroModel(path_to_config);
        gyro.initialize();
        data_bus = &bus;
    }

    void step() override {
        gyro.inputs.q_j2000_to_body_true = data_bus->fake_dynamics_outputs.q_fake;

        gyro.run();

        data_bus->gyro_outputs.measured_delta_angles = gyro.outputs.measured_delta_angles;
        data_bus->gyro_outputs.measurement_valid     = gyro.outputs.gyro_measurement_valid;
    }


private:
    // Need member variables so they persists across step() calls
    GyroModel gyro; 
    DataBus* data_bus;
};

#endif