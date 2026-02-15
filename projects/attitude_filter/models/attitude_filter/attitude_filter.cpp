#include "attitude_filter.hpp"

AttitudeFilter::AttitudeFilter(std::string path_to_config) {
    YAML::Node config_data = load_yaml_file(path_to_config);
    
    q_body_to_star_tracker     = get_yaml_value<std::array<double, 4>>(config_data, "q_body_to_star_tracker");
    q_body_to_gyro             = get_yaml_value<std::array<double, 4>>(config_data, "q_body_to_gyro");
    double attitude_pn         = get_yaml_value<double>(config_data, "attitude_process_noise");
    double gyro_bias_pn        = get_yaml_value<double>(config_data, "gyro_bias_process_noise");
    double gyro_misalign_pn    = get_yaml_value<double>(config_data, "gyro_misalignment_process_noise");
    double gyro_sf_pn          = get_yaml_value<double>(config_data, "gyro_scale_factor_process_noise");
    double attitude_covar      = get_yaml_value<double>(config_data, "attitude_covariance");
    double gyro_bias_covar     = get_yaml_value<double>(config_data, "gyro_bias_covariance");
    double gyro_misalign_covar = get_yaml_value<double>(config_data, "gyro_misalignment_covariance");
    double gyro_sf_covar       = get_yaml_value<double>(config_data, "gyro_scale_factor_covariance");
    double st_x_meas_noise     = get_yaml_value<double>(config_data, "star_tracker_x_noise_1_sigma_deg") * deg2rad;
    double st_y_meas_noise     = get_yaml_value<double>(config_data, "star_tracker_y_noise_1_sigma_deg") * deg2rad;
    double st_z_meas_noise     = get_yaml_value<double>(config_data, "star_tracker_z_noise_1_sigma_deg") * deg2rad;

    q_gyro_to_body = q_body_to_gyro.inv();
    q_st_to_body   = q_body_to_star_tracker.inv();


    Q.setIdentity();
    Q(0,0)   = attitude_pn;
    Q(1,1)   = attitude_pn;
    Q(2,2)   = attitude_pn;
    Q(3,3)   = gyro_bias_pn;
    Q(4,4)   = gyro_bias_pn;
    Q(5,5)   = gyro_bias_pn;
    Q(6,6)   = gyro_misalign_pn;
    Q(7,7)   = gyro_misalign_pn;
    Q(8,8)   = gyro_misalign_pn;
    Q(9,9)   = gyro_sf_pn;
    Q(10,10) = gyro_sf_pn;
    Q(11,11) = gyro_sf_pn;

    P.setIdentity();
    P(0,0)   = attitude_covar;
    P(1,1)   = attitude_covar;
    P(2,2)   = attitude_covar;
    P(3,3)   = gyro_bias_covar;
    P(4,4)   = gyro_bias_covar;
    P(5,5)   = gyro_bias_covar;
    P(6,6)   = gyro_misalign_covar;
    P(7,7)   = gyro_misalign_covar;
    P(8,8)   = gyro_misalign_covar;
    P(9,9)   = gyro_sf_covar;
    P(10,10) = gyro_sf_covar;
    P(11,11) = gyro_sf_covar;

    R.setIdentity();
    R(0,0) = st_x_meas_noise * st_x_meas_noise;
    R(1,1) = st_y_meas_noise * st_y_meas_noise;
    R(2,2) = st_z_meas_noise * st_z_meas_noise;

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

    if (st_meas_valid == false) {
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
    st_meas_valid      = inputs.star_tracker_meas_valid;
    q_j2000_to_st_meas = inputs.q_j2000_to_star_tracker_meas;
}

void AttitudeFilter::process_gyro_meas() {
    gyro_delta_thetas = q_gyro_to_body * gyro_delta_thetas;

    S = {est_sf(0),      -est_misalign(2), est_misalign(1), 
         est_misalign(2), est_sf(1),      -est_misalign(0), 
        -est_misalign(1), est_misalign(0), est_sf(2)};

    bias_corrected_delta_thetas = gyro_delta_thetas - est_biases;
    corrected_delta_thetas      = (I3 - S) * bias_corrected_delta_thetas;
    q_gyro                      = to_quat(corrected_delta_thetas);
}

void AttitudeFilter::propagate_states() {
    q_j2000_to_body_est = q_gyro * q_j2000_to_body_est;
    q_j2000_to_body_est.normalize();

    rot_vec<double> omega_hat = corrected_delta_thetas;
    rot_vec<double> omega_bar = bias_corrected_delta_thetas;                

    dt            = time_now_sec - time_prev_sec;
    time_prev_sec = time_now_sec;

    stm.setIdentity();

    // Set first block for attitude
    // Should be: (I3 - skew(omega_hat))
    stm(0,1) =  omega_hat(2);
    stm(0,2) = -omega_hat(1);
    stm(1,2) =  omega_hat(0);
    stm(1,0) = -omega_hat(2);
    stm(2,0) =  omega_hat(1);
    stm(2,1) = -omega_hat(0);

    // Set second block for gyro biases 
    matrix<double, 3,3> temp = -(I3 - S) * dt;
    
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
    stm(0,7) = -omega_bar(2);
    stm(0,8) =  omega_bar(1);
    stm(1,8) = -omega_bar(0);
    stm(1,6) =  omega_bar(2);
    stm(2,6) = -omega_bar(1);
    stm(2,7) =  omega_bar(0);

    // Set fourth block for gyro scale factors
    stm(0,9)  = -omega_bar(0);
    stm(1,10) = -omega_bar(1);
    stm(2,11) = -omega_bar(2);

    P = stm * P * stm.transpose() + Q * dt;
}

void AttitudeFilter::process_star_tracker_meas() {
    q_j2000_to_body_meas = q_st_to_body * q_j2000_to_st_meas;
}

void AttitudeFilter::compute_residual() {
    quat<double> q_est_to_meas = q_j2000_to_body_meas * q_j2000_to_body_est.inv();
    rot_vec_residual = to_rot_vec(q_est_to_meas);
}

void AttitudeFilter::update_state() {
    matrix<double, 12,3> K = P * H_T * (H * P * H_T + R).inv();
    vector<double, 12> estimated_error = K * rot_vec_residual;
    P = (I12 - K*H) * P * (I12 - K*H).transpose() + K*R*K.transpose();

    // It would be great to be able to access a certain amount of the vector elements
    rot_vec<double> rot_vec_error = {estimated_error(0), estimated_error(1), estimated_error(2)};
    quat<double> q_error          = to_quat(rot_vec_error); 
    q_j2000_to_body_est           = q_error * q_j2000_to_body_est;

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
