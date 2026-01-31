#ifndef GYRO_SIM_APP_HPP
#define GYRO_SIM_APP_HPP

#include <memory>
#include <iostream>
#include <string>

#include "../../../sim_framework/sim_includes.hpp"
#include "../../../projects/attitude_filter/models/gyro_model.hpp"
#include "../../../projects/attitude_filter/sim_files/data_bus.hpp"

class GyroSimApp : public SimAppBase<DataBus> {
public:
    using SimAppBase::SimAppBase;

    void configure_model(const std::string& path_to_config, SimControl& sim_ctrl) override {
        uint64_t seed = sim_ctrl.get_seed();  
        
        gyro = GyroModel(path_to_config);
        gyro.initialize(seed);
    }

    void step(DataBus& bus, SimControl& sim_ctrl) override {
        (void)sim_ctrl; // Tells the compiler I know this varible is unused

        gyro.inputs.q_j2000_to_body_true = bus.fake_dynamics_outputs.q_fake;

        gyro.run();

        bus.gyro_outputs.measured_delta_angles = gyro.outputs.measured_delta_angles;
        bus.gyro_outputs.measurement_valid     = gyro.outputs.gyro_measurement_valid;
    }
    
    void teardown(DataBus& bus, SimControl& sim_ctrl) override {
        (void)bus;
        (void)sim_ctrl;
    }

private:
    GyroModel gyro; 
    int count = 0;
};

#endif