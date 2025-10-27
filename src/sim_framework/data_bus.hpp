#ifndef DATABUS_HPP
#define DATABUS_HPP

#include "math_utilities/math.hpp"
#include <array>

struct FakeDynamicsData {
    quat<double> q_fake = {0.5, -0.5, -0.5, 0.5};
};

struct GyroData {
    vector<double, 3> measured_delta_angles;
    bool measurement_valid;
};

class DataBus {
public:
    FakeDynamicsData fake_dynamics_outputs;
    GyroData gyro_outputs;
};

#endif
