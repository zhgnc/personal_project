#ifndef DATABUS_HPP
#define DATABUS_HPP

#include "math/math.hpp"
#include <array>

struct DataBus {
    struct FakeDynamicsData {
        quat<double> quat;
        vector<double, 3> body_rates;
        rot_vec<double> rot_vec_attitude;
    };

    struct GyroData {
        bool measurement_valid;
        vector<double, 3> measured_delta_angles;
        vector<double, 3> total_delta_angle_error;
        vector<double, 3> angle_biases;
        vector<double, 3> scale_factors;
        vector<double, 3> misalignments;
        uint64_t seed;
    };

    struct StarTrackerData {
        bool measurement_valid;
        quat<double> q_j2000_to_star_tracker_meas;
        vector<double, 3> measurement_error_rad;
        uint64_t seed;
    };

    struct AttitudeFilterData {
        quat<double> q_j2000_to_body_est;
        vector<double, 3> corrected_gyro_delta_thetas;
        vector<double, 3> est_gyro_biases_rps;
        vector<double, 3> est_gyro_to_st_misalignments_rad;
        vector<double, 3> est_gyro_scale_factors;
        rot_vec<double> rot_vec_residual;
    };


    FakeDynamicsData fake_dynamics_outputs;
    GyroData gyro_outputs;
    StarTrackerData star_tracker_outputs;
    AttitudeFilterData attitude_filter_outputs;
};

#endif
