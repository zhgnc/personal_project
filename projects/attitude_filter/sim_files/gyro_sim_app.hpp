#ifndef GYRO_SIM_APP_HPP
#define GYRO_SIM_APP_HPP

#include <memory>
#include <iostream>
#include <string>

#include "../../../sim_framework/sim_includes.hpp"
#include "../../../projects/attitude_filter/models/gyro/gyro_model.hpp"
#include "../../../projects/attitude_filter/models/gyro/gyro_model_config_loader.hpp"
#include "../../../projects/attitude_filter/sim_files/data_bus.hpp"

class GyroSimApp : public SimAppBase<DataBus> {
public:
    using SimAppBase::SimAppBase;

    void configure_model(const std::string& path_to_config, SimControl& sim_ctrl) override {
        uint64_t dispersion_seed = sim_ctrl.get_seed();
        uint64_t model_seed      = sim_ctrl.get_seed();

        gyro_config config = load_gyro_config(path_to_config, dispersion_seed, model_seed);
        
        gyro = GyroModel(config);
    }

    void step(DataBus& bus, SimControl& sim_ctrl) override {
        gyro.inputs.q_j2000_to_body_true = bus.fake_dynamics_outputs.quat;
        gyro.inputs.current_time_sec     = sim_ctrl.public_sim_data().current_sim_time_sec;

        gyro.run();

        bus.gyro_outputs.measurement_valid       = gyro.outputs.gyro_measurement_valid;
        bus.gyro_outputs.measurement_time        = gyro.outputs.measurement_time;
        bus.gyro_outputs.measured_delta_angles   = gyro.outputs.measured_delta_angles;
        bus.gyro_outputs.total_delta_angle_error = gyro.outputs.total_delta_angle_error;
        bus.gyro_outputs.rate_biases            = gyro.outputs.rate_biases;
        bus.gyro_outputs.scale_factors           = gyro.outputs.scale_factors;
        bus.gyro_outputs.misalignments           = gyro.outputs.misalignments;
        bus.gyro_outputs.seed                    = gyro.outputs.seed;
    }
    
    void teardown(DataBus& bus, SimControl& sim_ctrl) override {
        (void)bus; // Tells compiler that I know these variables are unused
        (void)sim_ctrl;
    }

private:
    GyroModel gyro; 
};

#endif