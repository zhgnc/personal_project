#include "attitude_filter_config_loader.hpp"

AttitudeFilterConfig load_attitude_filter_config(const std::string& config_file_path) {
    YAML::Node config_file = load_yaml_file(config_file_path);

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

    return config_data;
}