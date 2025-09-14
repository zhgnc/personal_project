#ifndef TO_ROT_VEC_INL
#define TO_ROT_VEC_INL

template<typename T>
rot_vec<T> to_rot_vec(const quat<T>& quaternion) {
    T value = std::clamp(quaternion(3), T(-1.0), T(1.0));
    T angle          = 2.0 * std::acos(value); 
    vector<T,3> axis = {0.0, 0.0, 0.0};

    T sine_of_half_angle     = std::sqrt(1.0 - value*value); // More numerically stable than std::sin(angle / 2.0);
    const T numerical_limits = 100.0 * std::numeric_limits<T>::epsilon();

    if (sine_of_half_angle > numerical_limits) {
        axis = quaternion.vec() / sine_of_half_angle;
        axis = axis.norm();
    }

    T pi = std::numbers::pi;    
    if (angle > pi) {
        angle = 2*pi - angle;
        axis  = -axis; 
    }
    
    return rot_vec<T>(axis, angle); 
}

template<typename T>
rot_vec<T> to_rot_vec(const matrix<T,3,3>& dcm) {
    T value          = 0.5 * (dcm.trace() - 1.0);
    value            = std::clamp(value, T(-1.0), T(1.0));
    T angle          = std::acos(value);
    vector<T,3> axis = {0.0, 0.0, 0.0};

    const T numerical_limits = 100.0 * std::numeric_limits<T>::epsilon();
    if (angle > numerical_limits) {
        T constant = 1.0 / (2.0 * std::sin(angle));
        axis(0) = constant * (dcm(1,2) - dcm(2,1));
        axis(1) = constant * (dcm(2,0) - dcm(0,2));
        axis(2) = constant * (dcm(0,1) - dcm(1,0));
    }

    return rot_vec<T>(axis, angle);
}

#endif