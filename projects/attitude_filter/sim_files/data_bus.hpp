#ifndef DATABUS_HPP
#define DATABUS_HPP

#include "math/math.hpp"
#include <array>

struct DataBus {
    struct FakeDynamicsData {
        quat<double> q_fake = {0.5, -0.5, -0.5, 0.5};
    };

    struct GyroData {
        bool measurement_valid;
        vector<double, 3> measured_delta_angles;
        vector<double, 3> angle_biases;
        vector<double, 3> scale_factors;
        vector<double, 3> misalignments;
        uint64_t seed;
    };


    FakeDynamicsData fake_dynamics_outputs;
    GyroData gyro_outputs;
};

#endif
