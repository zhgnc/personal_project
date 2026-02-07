#include "attitude_filter.hpp"

AttitudeFilter::AttitudeFilter(std::string path_to_config) {
    (void)path_to_config;

    est_biases.setZeros();
    est_sf.setZeros();
    est_misalign.setZeros();

    I3.setIdentity();
}

void AttitudeFilter::run() {
    get_input_data();

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
    S = {est_sf(0),      -est_misalign(0), est_misalign(1), 
         est_misalign(0), est_sf(1),      -est_misalign(2), 
        -est_misalign(1), est_misalign(2), est_sf(2)};

    bias_corrected_delta_thetas = gyro_delta_thetas - est_biases;
    corrected_delta_thetas      = (I3 - S) * bias_corrected_delta_thetas;
    q_gyro                      = to_quat(corrected_delta_thetas);
}



void AttitudeFilter::populate_output_data() {
    outputs.corrected_gyro_delta_thetas      = corrected_delta_thetas;
    outputs.est_gyro_biases_rps              = est_biases;
    outputs.est_gyro_scale_factors           = est_sf;
    outputs.est_gyro_to_st_misalignments_rad = est_misalign;
    outputs.q_j2000_to_body_est              = q_j2000_to_body_est; 
}
