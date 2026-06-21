#ifndef ATTITUDE_FILTER_HPP
#define ATTITUDE_FILTER_HPP

#include "string"
#include <cmath>

#include "attitude_filter_struct_defs.hpp"
#include "utilities/yaml_utilities.hpp"

class AttitudeFilter
{
public:
    AttitudeFilter() = default; 
    AttitudeFilter(const AttitudeFilterConfig& config_data);

    void run();

    AttitudeFilterInputs inputs;
    AttitudeFilterOutputs outputs;
    AttitudeFilterConfig config;

private:
    void get_input_data();
    void process_gyro_meas();
    void process_star_tracker_meas();
    void propagate_states();
    void compute_residual();
    void update_state();
    void populate_output_data();

    // Constants
    quat<double> q_gyro_to_body;
    quat<double> q_st_to_body;
    matrix<double, 12,12> Q;
    matrix<double, 12,12> P;
    matrix<double, 3,3>   R;
    matrix<double, 3,12>  H;
    matrix<double, 12,3>  H_T;
    
    // Inputs 
    quat<double> q_j2000_to_st_meas;
    bool st_meas_valid;
    rot_vec<double> gyro_delta_thetas;
    bool gyro_valid_meas;
    double time_now_sec;
    double gyro_meas_time;
    double st_meas_time;

    // Outputs
    quat<double> q_j2000_to_body_est;
    rot_vec<double> corrected_delta_thetas;
    vector<double, 3> est_biases;
    vector<double, 3> est_sf;
    vector<double, 3> est_misalign;
    rot_vec<double> rot_vec_residual;

    // Private class members
    bool first_cycle;
    rot_vec<double> bias_corrected_delta_thetas;
    matrix<double, 3,3> S;
    quat<double> q_gyro;
    quat<double> q_j2000_to_body_meas;
    matrix<double, 12,12> stm;
    double time_prev_sec;
    double dt;
    matrix<double, 3,3> I3;
    matrix<double, 12,12> I12;
    double deg2rad = M_PI / 180.0;
};


#endif 