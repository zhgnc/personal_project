#include "attitude_filter.hpp"

AttitudeFilter::AttitudeFilter(std::string path_to_config) {
    YAML::Node config_data = load_yaml_file(path_to_config);
    
    q_body_to_star_tracker     = get_yaml_value<std::array<double, 4>>(config_data, "q_body_to_star_tracker");
    q_body_to_gyro             = get_yaml_value<std::array<double, 4>>(config_data, "q_body_to_gyro");
    double attitude_pn         = get_yaml_value<double>(config_data, "attitude_process_noise");
    double gyro_bias_pn        = get_yaml_value<double>(config_data, "gyro_bias_process_noise");
    double gyro_misalign_pn    = get_yaml_value<double>(config_data, "gyro_misalignment_process_noise");
    double gyro_sf_pn          = get_yaml_value<double>(config_data, "gyro_scale_factor_process_noise");
    double attitude_covar      = get_yaml_value<double>(config_data, "attitude_process_noise");
    double gyro_bias_covar     = get_yaml_value<double>(config_data, "gyro_bias_process_noise");
    double gyro_misalign_covar = get_yaml_value<double>(config_data, "gyro_misalignment_process_noise");
    double gyro_sf_covar       = get_yaml_value<double>(config_data, "gyro_scale_factor_process_noise");

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
    Q(0,0)   = attitude_covar;
    Q(1,1)   = attitude_covar;
    Q(2,2)   = attitude_covar;
    Q(3,3)   = gyro_bias_covar;
    Q(4,4)   = gyro_bias_covar;
    Q(5,5)   = gyro_bias_covar;
    Q(6,6)   = gyro_misalign_covar;
    Q(7,7)   = gyro_misalign_covar;
    Q(8,8)   = gyro_misalign_covar;
    Q(9,9)   = gyro_sf_covar;
    Q(10,10) = gyro_sf_covar;
    Q(11,11) = gyro_sf_covar;

    q_j2000_to_body_est.setIdentity();
    est_biases.setZeros();
    est_sf.setZeros();
    est_misalign.setZeros();

    I3.setIdentity();

    first_cycle = true;
}

void AttitudeFilter::run() {
    get_input_data();

    if (first_cycle == true) {
        first_cycle   = false;
        time_prev_sec = time_now_sec;
        return;
    }

    process_gyro_meas();
    propagate_states();

    if (st_meas_valid == false) {
        return;
    }

    process_star_tracker_meas();



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
    gyro_delta_thetas = q_body_to_gyro.inv() * gyro_delta_thetas;

    S = {est_sf(0),      -est_misalign(0), est_misalign(1), 
         est_misalign(0), est_sf(1),      -est_misalign(2), 
        -est_misalign(1), est_misalign(2), est_sf(2)};

    bias_corrected_delta_thetas = gyro_delta_thetas - est_biases;
    corrected_delta_thetas      = (I3 - S) * bias_corrected_delta_thetas;
    q_gyro                      = to_quat(corrected_delta_thetas);
}

void AttitudeFilter::propagate_states() {
    q_j2000_to_body_est = q_gyro * q_j2000_to_body_est;

    const rot_vec<double>& omega_hat = corrected_delta_thetas;
    const rot_vec<double>& omega_bar = bias_corrected_delta_thetas;

    dt            = time_now_sec - time_prev_sec;
    time_prev_sec = time_now_sec;

    stm.setIdentity();

    // Set first block for attitude
    stm(0,1) =  omega_hat(0);
    stm(0,2) = -omega_hat(1);
    stm(1,2) =  omega_hat(2);
    stm(1,0) = -omega_hat(0);
    stm(2,0) =  omega_hat(1);
    stm(2,1) = -omega_hat(2);

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
    stm(0,7) = -omega_bar(0);
    stm(0,8) =  omega_bar(1);
    stm(1,8) = -omega_bar(2);
    stm(1,6) =  omega_bar(0);
    stm(2,6) = -omega_bar(1);
    stm(2,7) =  omega_bar(2);

    // Set fourth block for gyro scale factors
    stm(0,9)  = -omega_bar(0);
    stm(1,10) = -omega_bar(1);
    stm(2,11) = -omega_bar(2);

    stm.print();
    omega_bar.print();
    omega_hat.print();

    P = stm * P * stm.transpose() + Q;
}

void AttitudeFilter::process_star_tracker_meas() {
    q_j2000_to_body_meas = q_body_to_star_tracker.inv() * q_j2000_to_st_meas;
}



void AttitudeFilter::populate_output_data() {
    outputs.corrected_gyro_delta_thetas      = corrected_delta_thetas;
    outputs.est_gyro_biases_rps              = est_biases;
    outputs.est_gyro_scale_factors           = est_sf;
    outputs.est_gyro_to_st_misalignments_rad = est_misalign;
    outputs.q_j2000_to_body_est              = q_j2000_to_body_est; 
}
