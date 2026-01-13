#ifndef GYRO_SIM_APP_HPP
#define GYRO_SIM_APP_HPP

#include <memory>
#include <iostream>
#include <string>

#include "../../../src/sim_framework/sim_app_base.hpp"
#include "../../../src/sensor_models/gyro/gyro_model.hpp"
#include "../../../examples/attitude_filter/apps/data_bus.hpp"

class GyroSimApp : public SimAppBase<DataBus> {
public:
    using SimAppBase::SimAppBase;

    void configure_model(const std::string& path_to_config, SimulationControl& sim_ctrl) override {
        (void)sim_ctrl;  // Tells the compiler I know this varible is unused
        
        gyro = GyroModel(path_to_config);
        gyro.initialize();
    }

    void step(DataBus& bus, SimulationControl& sim_ctrl) override {
        (void)sim_ctrl;

        gyro.inputs.q_j2000_to_body_true = bus.fake_dynamics_outputs.q_fake;

        gyro.run();

        bus.gyro_outputs.measured_delta_angles = gyro.outputs.measured_delta_angles;
        bus.gyro_outputs.measurement_valid     = gyro.outputs.gyro_measurement_valid;

        // count++;

        // if (count > 10) {
        //     // sim_ctrl.end_sim_after_app(StopReason::AlgorithmFailure, "Testing End Sim Now Function");
        //     sim_ctrl.end_sim_after_cycle(StopReason::ReachedEndObjective, "Testing End Sim After Cycle Function");
        //     count = 0;
        // }
    }

private:
    GyroModel gyro; 
    int count = 0;
};

#endif