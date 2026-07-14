#ifndef STAR_TRACKER_STRUCT_DEFS_HPP
#define STAR_TRACKER_STRUCT_DEFS_HPP

#include "math/math.hpp"

struct star_tracker_inputs {
    quat<double> q_j2000_to_body_true;
    vector<double, 3> body_rates_true_rps;
    double current_time_sec;
};

struct star_tracker_outputs {
    bool star_tracker_measurement_valid;
    quat<double> q_j2000_to_star_tracker_meas;
    vector<double, 3> measurement_error_rad;
    double measurement_time;
    uint64_t seed;
};

struct star_tracker_config {
    double x_axis_noise_rad_1_sigma;
    double y_axis_noise_rad_1_sigma;
    double z_axis_noise_rad_1_sigma;
    double max_rate_rps;
    quat<double> q_body_to_star_tracker;
    double manual_outage_start_sec;
    double manual_outage_stop_sec;
    bool outages_enabled;
    uint64_t random_seed;
};

#endif