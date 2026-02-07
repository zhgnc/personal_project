#ifndef ATTITUDE_FILTER_HPP
#define ATTITUDE_FILTER_HPP

#include "string"

#include "attitude_filter_struct_defs.hpp"

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

    // Inputs data
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

    // Private class members
    rot_vec<double> bias_corrected_delta_thetas;
    quat<double> q_gyro;
    matrix<double, 3,3> S;
    matrix<double, 3,3> I3;

};


#endif 