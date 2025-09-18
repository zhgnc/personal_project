#ifndef GYRO_STRUCT_DEFS_HPP
#define GYRO_STRUCT_DEFS_HPP

struct gyro_config {
    double angle_random_walk_1_sigma = 1.0; 
    double rate_random_walk_1_sigma  = 2.0;
    double scale_factor_1_sigma      = 3.0;  
    double misalignment_1_sigma      = 4.0;
    double rate_hz                   = 5.0; 
};

#endif