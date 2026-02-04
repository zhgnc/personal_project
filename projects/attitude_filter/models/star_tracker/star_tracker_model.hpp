#ifndef STAR_TRACKER_MODEL_HPP
#define STAR_TRACKER_MODEL_HPP

#include "math/math.hpp"
#include "utilities/yaml_utilities.hpp"
#include "star_tracker_struct_defs.hpp"

#include <random>

class StarTrackerModel 
{
public:
    StarTrackerModel() = default;
    StarTrackerModel(const std::string& config_file, uint64_t seed);

    void run();

    star_tracker_inputs  inputs; 
    star_tracker_outputs outputs;

private:
    void initialize();
    void copy_inputs_to_class(); 
    void execute(); 
    void set_outputs();

    double x_axis_noise_rad_1_sigma;
    double y_axis_noise_rad_1_sigma;
    double z_axis_noise_rad_1_sigma;
    quat<double> q_body_to_star_tracker;
    double max_rate_rps;
    uint64_t random_seed;

    quat<double> true_q_j2000_to_body;
    vector<double, 3> true_body_rates_rps;
    double time_now_sec;

    quat<double> q_j2000_to_star_tracker_meas;
    bool star_tracker_meas_valid;

    rot_vec<double> per_axis_noise_rad_1_sigma;
    rot_vec<double> noise;
    quat<double> q_noise;
        
    static constexpr double deg2rad = M_PI / 180.0;

    std::mt19937 rng;
    std::normal_distribution<> normal_distribution;
};

#endif