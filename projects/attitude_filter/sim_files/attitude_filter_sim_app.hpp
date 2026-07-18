#ifndef af_SIM_APP_HPP
#define af_SIM_APP_HPP

#include <memory>
#include <iostream>
#include <string>
#include <cmath>

#include "../../../sim_framework/sim_includes.hpp"
#include "../../../projects/attitude_filter/models/attitude_filter/attitude_filter.hpp"

class AttitudeFilterSimApp : public SimAppBase {
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

    void declare_io(IoRegistry& io) override {
        // ---- Measurement inputs, copied into the model before each step ----
        // The gyro publishes delta angles as vector<double,3> while the model
        // input is rot_vec<double>, so this one lands in an app member and is
        // converted in step() (the old data bus did this conversion silently)
        io.subscribe("meas_delta_thetas",            meas_delta_angles, Units::Radians);
        io.subscribe("gyro_meas_valid",              af.inputs.gyro_meas_valid);
        io.subscribe("gyro_meas_time",               af.inputs.gyro_meas_time, Units::Seconds);
        io.subscribe("q_j2000_to_star_tracker_meas", af.inputs.q_j2000_to_star_tracker_meas);
        io.subscribe("star_tracker_meas_valid",      af.inputs.star_tracker_meas_valid);
        io.subscribe("star_tracker_meas_time",       af.inputs.star_tracker_meas_time, Units::Seconds);

        // ---- Truth inputs used only for performance evaluation; subscribed
        // ---- into app members, not into the filter model itself ----
        io.subscribe("q_j2000_to_body_true", q_j2000_to_body_true);
        io.subscribe("true_rate_biases",     true_rate_biases,   Units::RadiansPerSec);
        io.subscribe("true_misalignments",   true_misalignments, Units::Radians);
        io.subscribe("true_scale_factors",   true_scale_factors, Units::Dimensionless);

        // ---- Filter outputs ----
        io.tlm_req("q_j2000_to_body_est",              af.outputs.q_j2000_to_body_est);
        io.tlm_req("est_gyro_biases_rad",              af.outputs.est_gyro_biases_rad);
        io.tlm_req("est_gyro_to_st_misalignments_rad", af.outputs.est_gyro_to_st_misalignments_rad);
        io.tlm_req("est_gyro_scale_factors",           af.outputs.est_gyro_scale_factors);
        io.tlm_req("covariance_diagonals",             af.outputs.covariance_diagonals);
        io.tlm_req("current_time_sec",                 af.outputs.time_now_sec, Units::Seconds);

        io.debug_tlm("corrected_gyro_delta_thetas", af.outputs.corrected_gyro_delta_thetas, Units::Radians);
        io.debug_tlm("rot_vec_residual",            af.outputs.rot_vec_residual,            Units::Radians);

        // ---- Filter performance vs subscribed truth ----
        io.tlm_req("rot_vec_error",      rot_vec_error, Units::Radians);
        io.tlm_req("bias_error",         bias_error,    Units::RadiansPerSec);
        io.tlm_req("misalignment_error", misalignment_error, Units::Radians);
        io.tlm_req("scale_factor_error", scale_factor_error, Units::Dimensionless);
    }

    void step(SimControl& sim_ctrl) override {
        // All measurement and truth inputs are filled through subscribed
        // ports before this function runs
        af.inputs.meas_delta_thetas = meas_delta_angles;
        af.inputs.current_time_sec  = sim_ctrl.public_sim_data().current_sim_time_sec;

        af.run();

        // Calculate error between true and estimated states
        quat<double> q_true_to_est = (af.outputs.q_j2000_to_body_est * q_j2000_to_body_true.inv()).normalize();

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

        rot_vec_error      = to_rot_vec(q_true_to_est);
        bias_error         = true_rate_biases   - af.config.q_body_to_gyro * af.outputs.est_gyro_biases_rad;
        misalignment_error = true_misalignments - af.config.q_body_to_gyro * af.outputs.est_gyro_to_st_misalignments_rad;
        scale_factor_error = true_scale_factors - filter_sf_in_gyro;
    }

    void teardown(SimControl& sim_ctrl) override {
        (void)sim_ctrl;
    }

private:
    AttitudeFilter af;

    // Gyro delta angles subscribed here, converted to rot_vec in step()
    vector<double, 3> meas_delta_angles;

    // Truth values subscribed for performance evaluation
    quat<double>      q_j2000_to_body_true;
    vector<double, 3> true_rate_biases;
    vector<double, 3> true_misalignments;
    vector<double, 3> true_scale_factors;

    // Published performance metrics
    rot_vec<double>   rot_vec_error;
    vector<double, 3> bias_error;
    vector<double, 3> misalignment_error;
    vector<double, 3> scale_factor_error;
};

#endif
