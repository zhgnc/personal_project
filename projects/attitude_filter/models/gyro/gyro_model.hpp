#ifndef GYRO_MODEL_HPP
#define GYRO_MODEL_HPP

#include "math/math.hpp"
#include "utilities/yaml_utilities.hpp"
#include "gyro_struct_defs.hpp"

#include <random>

class GyroModel 
{
public:
    GyroModel() = default;
    GyroModel(const gyro_config& config_data);

    void run();

    gyro_inputs inputs; 
    gyro_outputs outputs;
    gyro_config config;

private:
    void copy_inputs_to_class(); 
    void execute(); 
    void set_outputs();

    quat<double> q_j2000_to_body_now; 
    quat<double> q_j2000_to_body_prev;
    quat<double> q_prev_to_now;

    double time_now_sec;
    double time_prev_sec;
    double dt;

    vector<double, 3> true_delta_angles;
    vector<double, 3> meas_delta_angles;
    vector<double, 3> total_delta_angle_error;
    vector<double, 3> scale_factors;
    vector<double, 3> misalignments;
    vector<double, 3> arw_error;
    vector<double, 3> rate_bias;

    bool first_cycle;
    bool gyro_meas_valid;
        
    matrix<double, 3,3> I3 = identity_matrix<double, 3>();
    matrix<double, 3,3> sf_misalign_matrix;

    static constexpr double deg2rad = M_PI / 180.0;

    std::mt19937 rng;
    std::normal_distribution<> normal_distribution;
};

#endif