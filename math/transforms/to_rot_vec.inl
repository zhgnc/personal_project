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
    quat<T> q = to_quat(dcm);
    return to_rot_vec(q);
}

#endif