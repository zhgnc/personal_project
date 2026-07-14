#ifndef TO_ROT_VEC_INL
#define TO_ROT_VEC_INL

template<typename type>
rot_vec<type> to_rot_vec(const quat<type>& quaternion) {
    type value = std::clamp(quaternion(3), type(-1.0), type(1.0));
    type angle          = 2.0 * std::acos(value); 
    vector<type,3> axis = {0.0, 0.0, 0.0};

    type sine_of_half_angle     = std::sqrt(1.0 - value*value); // More numerically stable than std::sin(angle / 2.0);
    const type numerical_limits = 100.0 * std::numeric_limits<type>::epsilon();

    if (sine_of_half_angle > numerical_limits) {
        axis = quaternion.vec() / sine_of_half_angle;
        axis = axis.norm();
    }

    type pi = std::numbers::pi;    
    if (angle > pi) {
        angle = 2*pi - angle;
        axis  = -axis; 
    }
    
    return rot_vec<type>(axis, angle); 
}

template<typename type>
rot_vec<type> to_rot_vec(const matrix<type,3,3>& dcm) {
    quat<type> q = to_quat(dcm);
    return to_rot_vec(q);
}

#endif