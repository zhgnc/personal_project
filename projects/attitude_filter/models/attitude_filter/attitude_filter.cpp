#include "attitude_filter.hpp"

AttitudeFilter::AttitudeFilter(std::string path_to_config) {
    YAML::Node config_data = load_yaml_file(path_to_config);
    
    config.q_body_to_star_tracker = get_yaml_value<std::array<double, 4>>(config_data, "q_body_to_star_tracker");
    config.q_body_to_gyro         = get_yaml_value<std::array<double, 4>>(config_data, "q_body_to_gyro");
    config.attitude_pn            = get_yaml_value<double>(config_data, "attitude_process_noise");
    config.gyro_bias_pn           = get_yaml_value<double>(config_data, "gyro_bias_process_noise");
    config.gyro_misalign_pn       = get_yaml_value<double>(config_data, "gyro_misalignment_process_noise");
    config.gyro_sf_pn             = get_yaml_value<double>(config_data, "gyro_scale_factor_process_noise");
    config.attitude_covar         = get_yaml_value<double>(config_data, "attitude_covariance");
    config.gyro_bias_covar        = get_yaml_value<double>(config_data, "gyro_bias_covariance");
    config.gyro_misalign_covar    = get_yaml_value<double>(config_data, "gyro_misalignment_covariance");
    config.gyro_sf_covar          = get_yaml_value<double>(config_data, "gyro_scale_factor_covariance");
    config.st_x_meas_noise        = get_yaml_value<double>(config_data, "star_tracker_x_noise_1_sigma_deg") * deg2rad;
    config.st_y_meas_noise        = get_yaml_value<double>(config_data, "star_tracker_y_noise_1_sigma_deg") * deg2rad;
    config.st_z_meas_noise        = get_yaml_value<double>(config_data, "star_tracker_z_noise_1_sigma_deg") * deg2rad;

    q_gyro_to_body = (config.q_body_to_gyro.normalize()).inv();
    q_st_to_body   = (config.q_body_to_star_tracker.normalize()).inv();


    Q.setIdentity();
    Q(0,0)   = config.attitude_pn;
    Q(1,1)   = config.attitude_pn;
    Q(2,2)   = config.attitude_pn;
    Q(3,3)   = config.gyro_bias_pn;
    Q(4,4)   = config.gyro_bias_pn;
    Q(5,5)   = config.gyro_bias_pn;
    Q(6,6)   = config.gyro_misalign_pn;
    Q(7,7)   = config.gyro_misalign_pn;
    Q(8,8)   = config.gyro_misalign_pn;
    Q(9,9)   = config.gyro_sf_pn;
    Q(10,10) = config.gyro_sf_pn;
    Q(11,11) = config.gyro_sf_pn;

    P.setIdentity();
    P(0,0)   = config.attitude_covar;
    P(1,1)   = config.attitude_covar;
    P(2,2)   = config.attitude_covar;
    P(3,3)   = config.gyro_bias_covar;
    P(4,4)   = config.gyro_bias_covar;
    P(5,5)   = config.gyro_bias_covar;
    P(6,6)   = config.gyro_misalign_covar;
    P(7,7)   = config.gyro_misalign_covar;
    P(8,8)   = config.gyro_misalign_covar;
    P(9,9)   = config.gyro_sf_covar;
    P(10,10) = config.gyro_sf_covar;
    P(11,11) = config.gyro_sf_covar;

    R.setIdentity();
    R(0,0) = config.st_x_meas_noise * config.st_x_meas_noise;
    R(1,1) = config.st_y_meas_noise * config.st_y_meas_noise;
    R(2,2) = config.st_z_meas_noise * config.st_z_meas_noise;

    matrix<double, 3,3> dcm_st_to_body = to_rotation_matrix(q_st_to_body);
    R = dcm_st_to_body * R * dcm_st_to_body.transpose(); 

    H.setZeros();
    H(0,0) = 1.0;
    H(1,1) = 1.0;
    H(2,2) = 1.0;

    H_T = H.transpose();

    q_j2000_to_body_est.setIdentity();
    est_biases.setZeros();
    est_sf.setZeros();
    est_misalign.setZeros();

    I3.setIdentity();
    I12.setIdentity();

    first_cycle = true;
}

void AttitudeFilter::run() {
    get_input_data();
    process_gyro_meas();
    process_star_tracker_meas();

    if (first_cycle == true) {
        first_cycle   = false;
        time_prev_sec = time_now_sec;
        return;
    }

    propagate_states();

    if (st_meas_valid == false || (st_meas_time - time_now_sec) < 0.0) {
        populate_output_data();
        return;
    }

    compute_residual();
    update_state();
    populate_output_data();
}

void AttitudeFilter::get_input_data() {
    time_now_sec       = inputs.current_time_sec;
    gyro_valid_meas    = inputs.gyro_meas_valid;
    gyro_delta_thetas  = inputs.meas_delta_thetas;
    gyro_meas_time     = inputs.gyro_meas_time;
    st_meas_valid      = inputs.star_tracker_meas_valid;
    q_j2000_to_st_meas = inputs.q_j2000_to_star_tracker_meas;
    st_meas_time       = inputs.star_tracker_meas_time;
}

void AttitudeFilter::process_gyro_meas() {
    dt            = time_now_sec - time_prev_sec;
    time_prev_sec = time_now_sec;
    
    gyro_delta_thetas = q_gyro_to_body * gyro_delta_thetas;

    S = {est_sf(0),      -est_misalign(2), est_misalign(1), 
         est_misalign(2), est_sf(1),      -est_misalign(0), 
        -est_misalign(1), est_misalign(0), est_sf(2)};

    bias_corrected_delta_thetas = gyro_delta_thetas - est_biases * dt; 
    corrected_delta_thetas      = (I3 + S).inv() * bias_corrected_delta_thetas; // (I3 - S) is approximation of (I3 + S).inv()
    q_gyro                      = to_quat(corrected_delta_thetas).normalize();
}

void AttitudeFilter::propagate_states() {
    q_j2000_to_body_est = (q_gyro * q_j2000_to_body_est).normalize();

    rot_vec<double> theta_hat = corrected_delta_thetas;
    rot_vec<double> theta_bar = bias_corrected_delta_thetas;
    
    stm.setIdentity();

    // Set first block for attitude
    // Should be: (I3 - skew(theta_hat))
    stm(0,1) =  theta_hat(2);
    stm(0,2) = -theta_hat(1);
    stm(1,2) =  theta_hat(0);
    stm(1,0) = -theta_hat(2);
    stm(2,0) =  theta_hat(1);
    stm(2,1) = -theta_hat(0);

    // Set second block for gyro biases 
    matrix<double, 3,3> temp = -(I3 + S).inv() * dt; // (I3 - S) is approximation of (I3 + S).inv()

    stm(0,3) = temp(0,0); // It would be nice to have a block assign feature....
    stm(0,4) = temp(0,1);
    stm(0,5) = temp(0,2);
    stm(1,3) = temp(1,0);
    stm(1,4) = temp(1,1);
    stm(1,5) = temp(1,2);
    stm(2,3) = temp(2,0);
    stm(2,4) = temp(2,1);
    stm(2,5) = temp(2,2);

    // Set third block for gyro to star tracker misalignments
    stm(0,7) = -theta_bar(2);
    stm(0,8) =  theta_bar(1);
    stm(1,8) = -theta_bar(0);
    stm(1,6) =  theta_bar(2);
    stm(2,6) = -theta_bar(1);
    stm(2,7) =  theta_bar(0);

    // Set fourth block for gyro scale factors
    stm(0,9)  = -theta_bar(0);
    stm(1,10) = -theta_bar(1);
    stm(2,11) = -theta_bar(2);

    P = stm * P * stm.transpose() + Q * dt;
}

void AttitudeFilter::process_star_tracker_meas() {
    q_j2000_to_body_meas = (q_st_to_body * q_j2000_to_st_meas).normalize();
}

void AttitudeFilter::compute_residual() {
    quat<double> q_est_to_meas = (q_j2000_to_body_meas * q_j2000_to_body_est.inv()).normalize();
    rot_vec_residual = to_rot_vec(q_est_to_meas);
}

void AttitudeFilter::update_state() {
    matrix<double, 12,3> K = P * H_T * (H * P * H_T + R).inv();
    vector<double, 12> estimated_error = K * rot_vec_residual;
    P = (I12 - K*H) * P * (I12 - K*H).transpose() + K*R*K.transpose();

    // It would be great to be able to access a certain amount of the vector elements
    rot_vec<double> rot_vec_error = {estimated_error(0), estimated_error(1), estimated_error(2)};
    quat<double> q_error          = to_quat(rot_vec_error).normalize(); 
    q_j2000_to_body_est           = (q_error * q_j2000_to_body_est).normalize();

    vector<double, 3> bias_errors = {estimated_error(3), estimated_error(4), estimated_error(5)};
    est_biases = est_biases + bias_errors;

    vector<double, 3> misalignment_errors = {estimated_error(6), estimated_error(7), estimated_error(8)};
    est_misalign = est_misalign + misalignment_errors;

    vector<double, 3> sf_errors = {estimated_error(9), estimated_error(10), estimated_error(11)};
    est_sf = est_sf + sf_errors;
}

void AttitudeFilter::populate_output_data() {
    outputs.corrected_gyro_delta_thetas      = corrected_delta_thetas;
    outputs.est_gyro_biases_rad              = est_biases;
    outputs.est_gyro_scale_factors           = est_sf;
    outputs.est_gyro_to_st_misalignments_rad = est_misalign;
    outputs.q_j2000_to_body_est              = q_j2000_to_body_est; 
    outputs.rot_vec_residual                 = rot_vec_residual;
    outputs.time_now_sec                     = time_now_sec;
    
    outputs.covariance_diagonals(0)          = P(0,0);
    outputs.covariance_diagonals(1)          = P(1,1);
    outputs.covariance_diagonals(2)          = P(2,2);
    outputs.covariance_diagonals(3)          = P(3,3);
    outputs.covariance_diagonals(4)          = P(4,4);
    outputs.covariance_diagonals(5)          = P(5,5);
    outputs.covariance_diagonals(6)          = P(6,6);
    outputs.covariance_diagonals(7)          = P(7,7);
    outputs.covariance_diagonals(8)          = P(8,8);
    outputs.covariance_diagonals(9)          = P(9,9);
    outputs.covariance_diagonals(10)         = P(10,10);
    outputs.covariance_diagonals(11)         = P(11,11);
}
