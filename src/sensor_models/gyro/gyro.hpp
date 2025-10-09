#ifndef GYRO_HPP
#define GYRO_HPP

#include "../../math_utilities/math.hpp"
#include "struct_defs.hpp"
#include <random>

class gyro 
{
public:
    gyro();
    gyro(const std::string& config_file);

    void initialize();
    void copy_inputs_to_class(); 
    void execute(); 
    void set_outputs();

    gyro_inputs  inputs; 
    gyro_outputs outputs;

private:
    gyro_default_config default_config;

    double init_bias_1_sigma;
    double arw_1_sigma; 
    double rrw_1_sigma; 
    double sf_1_sigma; 
    double misalign_1_sigma;
    double frequency;
    int random_seed;

    quat<double> q_j2000_to_body_now; 
    quat<double> q_j2000_to_body_prev;
    quat<double> q_prev_to_now;
    vector<double, 3> true_delta_angles;
    vector<double, 3> meas_delta_angles;

    vector<double, 3> initial_rate_biases;
    vector<double, 3> rate_biases;
    vector<double, 3> scale_factors;
    vector<double, 3> misalignments;

    double dt;
    vector<double, 3> arw_error;
    vector<double, 3> bias_error;
    matrix<double, 3,3> sf_misalign_matrix;
    matrix<double, 3,3> I3;

    bool gyro_meas_valid;

    std::mt19937 rng;
    std::normal_distribution<> normal_distribution;
};

#endif