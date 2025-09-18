#ifndef GYRO_CPP
#define GYRO_CPP

#include "gyro.hpp"

gyro::gyro() {
    arw_1_sigma      = config.angle_random_walk_1_sigma;
    rrw_1_sigma      = config.rate_random_walk_1_sigma;
    sf_1_sigma       = config.scale_factor_1_sigma; 
    misalign_1_sigma = config.misalignment_1_sigma;
    frequency        = config.rate_hz; 
}

void gyro::initialize() {
    
};

#endif