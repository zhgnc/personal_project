#ifndef GYRO_STRUCT_DEFS_HPP
#define GYRO_STRUCT_DEFS_HPP

#include "math/math.hpp"

struct gyro_inputs {
    quat<double> q_j2000_to_body_true;
    double current_time_sec;
};

struct gyro_outputs {
    bool gyro_measurement_valid;
    double measurement_time;
    vector<double, 3> measured_delta_angles;
    vector<double, 3> total_delta_angle_error;
    vector<double, 3> rate_biases;
    vector<double, 3> scale_factors;
    vector<double, 3> misalignments;
    uint64_t seed;
};

struct gyro_config {
    double arw_1_sigma;            
    vector<double, 3> init_bias_rps; 
    double rrw_1_sigma;            
    vector<double, 3> scale_factors;             
    vector<double, 3> misalignments_rad;       
    quat<double> q_body_to_gyro;         
    uint64_t random_seed;            
};

#endif