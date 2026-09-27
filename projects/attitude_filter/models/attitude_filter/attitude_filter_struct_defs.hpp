#ifndef ATTITUDE_FILTER_STRUCT_DEFS
#define ATTITUDE_FILTER_STRUCT_DEFS

#include "math/math.hpp"

// Scalar fields carry explicit initializers so the first telemetry sample
// (taken before the filter processes its first measurement) records a defined
// value instead of uninitialized memory
struct AttitudeFilterInputs {
    quat<double> q_j2000_to_star_tracker_meas;
    bool star_tracker_meas_valid = false;
    double star_tracker_meas_time = 0.0;
    rot_vec<double> meas_delta_thetas;
    bool gyro_meas_valid = false;
    double gyro_meas_time = 0.0;
    double current_time_sec = 0.0;
};

struct AttitudeFilterOutputs {
    quat<double> q_j2000_to_body_est;
    rot_vec<double> corrected_gyro_delta_thetas;
    vector<double, 3> est_gyro_biases_rad;
    vector<double, 3> est_gyro_to_st_misalignments_rad;
    vector<double, 3> est_gyro_scale_factors;
    rot_vec<double> rot_vec_residual;
    vector<double, 12> covariance_diagonals;
    double time_now_sec = 0.0;
};

struct AttitudeFilterConfig {
    quat<double> q_body_to_star_tracker; 
    quat<double> q_body_to_gyro; 
    double attitude_pn; 
    double gyro_bias_pn; 
    double gyro_misalign_pn; 
    double gyro_sf_pn; 
    double attitude_covar; 
    double gyro_bias_covar; 
    double gyro_misalign_covar; 
    double gyro_sf_covar; 
    double st_x_meas_noise; 
    double st_y_meas_noise; 
    double st_z_meas_noise; 
};

#endif