#ifndef af_SIM_APP_HPP
#define af_SIM_APP_HPP

#include <memory>
#include <iostream>
#include <string>
#include <cmath>

#include "../../../sim_framework/sim_includes.hpp"
#include "../../../projects/attitude_filter/models/attitude_filter/attitude_filter.hpp"
#include "../../../projects/attitude_filter/sim_files/data_bus.hpp"

class AttitudeFilterSimApp : public SimAppBase<DataBus> {
public:
    using SimAppBase::SimAppBase;

    void configure_model(const std::string& path_to_config, SimControl& sim_ctrl) override {
        (void)sim_ctrl;

        // Parse yaml config file
        YAML::Node config_file = load_yaml_file(path_to_config);

        AttitudeFilterConfig config_data;

        config_data.q_body_to_star_tracker = get_yaml_value<std::array<double, 4>>(config_file, "q_body_to_star_tracker");
        config_data.q_body_to_gyro         = get_yaml_value<std::array<double, 4>>(config_file, "q_body_to_gyro");
        config_data.attitude_pn            = get_yaml_value<double>(config_file, "attitude_process_noise");
        config_data.gyro_bias_pn           = get_yaml_value<double>(config_file, "gyro_bias_process_noise");
        config_data.gyro_misalign_pn       = get_yaml_value<double>(config_file, "gyro_misalignment_process_noise");
        config_data.gyro_sf_pn             = get_yaml_value<double>(config_file, "gyro_scale_factor_process_noise");

        config_data.attitude_covar         = get_yaml_value<double>(config_file, "attitude_covariance");
        config_data.gyro_bias_covar        = get_yaml_value<double>(config_file, "gyro_bias_covariance");
        config_data.gyro_misalign_covar    = get_yaml_value<double>(config_file, "gyro_misalignment_covariance");
        config_data.gyro_sf_covar          = get_yaml_value<double>(config_file, "gyro_scale_factor_covariance");

        config_data.st_x_meas_noise = get_yaml_value<double>(config_file, "star_tracker_x_noise_1_sigma_deg") * M_PI / 180.0;
        config_data.st_y_meas_noise = get_yaml_value<double>(config_file, "star_tracker_y_noise_1_sigma_deg") * M_PI / 180.0;
        config_data.st_z_meas_noise = get_yaml_value<double>(config_file, "star_tracker_z_noise_1_sigma_deg") * M_PI / 180.0;

        // Construct attitude filter with configuration data
        af = AttitudeFilter(config_data);
    }

    void step(DataBus& bus, SimControl& sim_ctrl) override {
        af.inputs.q_j2000_to_star_tracker_meas = bus.star_tracker_outputs.q_j2000_to_star_tracker_meas;
        af.inputs.star_tracker_meas_valid      = bus.star_tracker_outputs.measurement_valid;
        af.inputs.star_tracker_meas_time       = bus.star_tracker_outputs.measurement_time;
        af.inputs.meas_delta_thetas            = bus.gyro_outputs.measured_delta_angles;
        af.inputs.gyro_meas_valid              = bus.gyro_outputs.measurement_valid;
        af.inputs.gyro_meas_time               = bus.gyro_outputs.measurement_time;
        af.inputs.current_time_sec             = sim_ctrl.public_sim_data().current_sim_time_sec;

        af.run();

        bus.attitude_filter_outputs.q_j2000_to_body_est              = af.outputs.q_j2000_to_body_est;
        bus.attitude_filter_outputs.corrected_gyro_delta_thetas      = af.outputs.corrected_gyro_delta_thetas;
        bus.attitude_filter_outputs.est_gyro_biases_rad              = af.outputs.est_gyro_biases_rad;
        bus.attitude_filter_outputs.est_gyro_to_st_misalignments_rad = af.outputs.est_gyro_to_st_misalignments_rad;
        bus.attitude_filter_outputs.est_gyro_scale_factors           = af.outputs.est_gyro_scale_factors;
        bus.attitude_filter_outputs.rot_vec_residual                 = af.outputs.rot_vec_residual;
        bus.attitude_filter_outputs.covariance_diagonals             = af.outputs.covariance_diagonals;
        bus.attitude_filter_outputs.current_time_sec                 = af.outputs.time_now_sec;
        
        // Calculate error between true and estimated states
        quat<double> q_true_to_est = (af.outputs.q_j2000_to_body_est * bus.fake_dynamics_outputs.quat.inv()).normalize();

        // This is required because the scale factors are not vectors but diagonal elements of a matrix
        // These wolfram alpha links show the subtle difference
        // https://www.wolframalpha.com/input?i2d=true&i=%7B%7B1%2C0%2C0%7D%2C%7B0%2C0%2C-1%7D%2C%7B0%2C1%2C0%7D%7D%7B%7B0%7D%2C%7B0%7D%2C%7B1%7D%7D
        // https://www.wolframalpha.com/input?i2d=true&i=%7B%7B1%2C0%2C0%7D%2C%7B0%2C0%2C-1%7D%2C%7B0%2C1%2C0%7D%7D%7B%7B1%2C0%2C0%7D%2C%7B0%2C2%2C0%7D%2C%7B0%2C0%2C3%7D%7DPower%5B%7B%7B1%2C0%2C0%7D%2C%7B0%2C0%2C-1%7D%2C%7B0%2C1%2C0%7D%7D%2C-1%5D
        matrix<double, 3,3> S_body = {af.outputs.est_gyro_scale_factors(0), 0.0,  0.0, 
                                      0.0,  af.outputs.est_gyro_scale_factors(1), 0.0, 
                                      0.0, 0.0,  af.outputs.est_gyro_scale_factors(2)};

        matrix<double, 3,3> dcm_body_2_gyro   = to_rotation_matrix(af.config.q_body_to_gyro);
        matrix<double, 3,3> S_gyro            = dcm_body_2_gyro * S_body * dcm_body_2_gyro.T();
        vector<double, 3>   filter_sf_in_gyro = {S_gyro(0,0), S_gyro(1,1), S_gyro(2,2)};

        bus.attitude_filter_performance.rot_vec_error      = to_rot_vec(q_true_to_est);
        bus.attitude_filter_performance.bias_error         = bus.gyro_outputs.rate_biases   - af.config.q_body_to_gyro * af.outputs.est_gyro_biases_rad;  
        bus.attitude_filter_performance.misalignment_error = bus.gyro_outputs.misalignments - af.config.q_body_to_gyro * af.outputs.est_gyro_to_st_misalignments_rad;
        bus.attitude_filter_performance.scale_factor_error = bus.gyro_outputs.scale_factors - filter_sf_in_gyro;
    }
    
    void teardown(DataBus& bus, SimControl& sim_ctrl) override {
        (void)bus;
        (void)sim_ctrl;
    }

private:
    AttitudeFilter af; 
};

#endif