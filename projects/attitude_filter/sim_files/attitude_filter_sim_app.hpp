#ifndef ATTITUDE_FILTER_SIM_APP_HPP
#define ATTITUDE_FILTER_SIM_APP_HPP

#include <memory>
#include <iostream>
#include <string>

#include "../../../sim_framework/sim_includes.hpp"
#include "../../../projects/attitude_filter/models/attitude_filter/attitude_filter.hpp"
#include "../../../projects/attitude_filter/sim_files/data_bus.hpp"

class AttitudeFilterSimApp : public SimAppBase<DataBus> {
public:
    using SimAppBase::SimAppBase;

    void configure_model(const std::string& path_to_config, SimControl& sim_ctrl) override {
        (void)sim_ctrl;

        attitude_filter = AttitudeFilter(path_to_config);
    }

    void step(DataBus& bus, SimControl& sim_ctrl) override {
        attitude_filter.inputs.q_j2000_to_star_tracker_meas = bus.star_tracker_outputs.q_j2000_to_star_tracker_meas;
        attitude_filter.inputs.star_tracker_meas_valid      = bus.star_tracker_outputs.measurement_valid;
        attitude_filter.inputs.meas_delta_thetas            = bus.gyro_outputs.measured_delta_angles;
        attitude_filter.inputs.gyro_meas_valid              = bus.gyro_outputs.measurement_valid;
        attitude_filter.inputs.current_time_sec             = sim_ctrl.public_sim_data().current_sim_time_sec;

        attitude_filter.run();

        bus.attitude_filter_outputs.q_j2000_to_body_est              = attitude_filter.outputs.q_j2000_to_body_est;
        bus.attitude_filter_outputs.corrected_gyro_delta_thetas      = attitude_filter.outputs.corrected_gyro_delta_thetas;
        bus.attitude_filter_outputs.est_gyro_biases_rad              = attitude_filter.outputs.est_gyro_biases_rad;
        bus.attitude_filter_outputs.est_gyro_to_st_misalignments_rad = attitude_filter.outputs.est_gyro_to_st_misalignments_rad;
        bus.attitude_filter_outputs.est_gyro_scale_factors           = attitude_filter.outputs.est_gyro_scale_factors;
        bus.attitude_filter_outputs.rot_vec_residual                 = attitude_filter.outputs.rot_vec_residual;
        bus.attitude_filter_outputs.covariance_diagonals             = attitude_filter.outputs.covariance_diagonals;


        quat<double> q_true_to_est = (attitude_filter.outputs.q_j2000_to_body_est * bus.fake_dynamics_outputs.quat.inv()).normalize();

        bus.attitude_filter_performance.rot_vec_error      = to_rot_vec(q_true_to_est);
        bus.attitude_filter_performance.bias_error         = attitude_filter.outputs.est_gyro_biases_rad              - bus.gyro_outputs.angle_biases;
        bus.attitude_filter_performance.misalignment_error = attitude_filter.outputs.est_gyro_to_st_misalignments_rad - bus.gyro_outputs.misalignments;
        bus.attitude_filter_performance.scale_factor_error = attitude_filter.outputs.est_gyro_scale_factors           - bus.gyro_outputs.scale_factors;
    }
    
    void teardown(DataBus& bus, SimControl& sim_ctrl) override {
        (void)bus;
        (void)sim_ctrl;
    }

private:
    AttitudeFilter attitude_filter; 
};

#endif