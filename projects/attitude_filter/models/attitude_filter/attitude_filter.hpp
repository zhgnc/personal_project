#ifndef ATTITUDE_FILTER_HPP
#define ATTITUDE_FILTER_HPP

#include "string"

#include "attitude_filter_struct_defs.hpp"
#include "utilities/yaml_utilities.hpp"

class AttitudeFilter
{
public:
    AttitudeFilter() = default;
    AttitudeFilter(std::string path_to_config);

    void run();

    AttitudeFilterInputs inputs;
    AttitudeFilterOutputs outputs;

private:
    void get_input_data();
    void process_gyro_meas();
    void process_star_tracker_meas();
    void propagate_states();
    void compute_residual();
    void update_state();
    void populate_output_data();

    // Config 
    quat<double> q_body_to_star_tracker;
    quat<double> q_body_to_gyro;
    matrix<double, 12,12> Q;
    matrix<double, 12,12> P;
    matrix<double, 3,3>   R;
    
    // Inputs 
    quat<double> q_j2000_to_st_meas;
    bool st_meas_valid;
    rot_vec<double> gyro_delta_thetas;
    bool gyro_valid_meas;
    double time_now_sec;

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