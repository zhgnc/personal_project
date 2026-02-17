#ifndef ATTITUDE_FILTER_STRUCT_DEFS
#define ATTITUDE_FILTER_STRUCT_DEFS

#include "math/math.hpp"

struct AttitudeFilterInputs {
    quat<double> q_j2000_to_star_tracker_meas;
    bool star_tracker_meas_valid;
    double star_tracker_meas_time;
    rot_vec<double> meas_delta_thetas;
    bool gyro_meas_valid;
    double gyro_meas_time;
    double current_time_sec;
};

struct AttitudeFilterOutputs {
    quat<double> q_j2000_to_body_est;
    rot_vec<double> corrected_gyro_delta_thetas;
    vector<double, 3> est_gyro_biases_rad;
    vector<double, 3> est_gyro_to_st_misalignments_rad;
    vector<double, 3> est_gyro_scale_factors;
    rot_vec<double> rot_vec_residual;
    vector<double, 12> covariance_diagonals;
    double time_now_sec;
};

#endif