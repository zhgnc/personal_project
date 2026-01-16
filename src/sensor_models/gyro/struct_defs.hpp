#ifndef GYRO_STRUCT_DEFS_HPP
#define GYRO_STRUCT_DEFS_HPP

#include "../../math/math.hpp"

struct gyro_default_config {
    double turn_on_bias_1_sigma      = 0.0;
    double angle_random_walk_1_sigma = 1.0; 
    double rate_random_walk_1_sigma  = 2.0;
    double scale_factor_1_sigma      = 3.0;  
    double misalignment_1_sigma      = 4.0;
    double rate_hz                   = 5.0; 
    int    random_seed               = 6;
};

struct gyro_inputs {
    quat<double> q_j2000_to_body_true;
};

struct gyro_outputs {
    bool gyro_measurement_valid;
    vector<double, 3> measured_delta_angles;
};

#endif