#ifndef TO_QUAT_INL
#define TO_QUAT_INL

#include "transforms.hpp"

template<typename T>
quat<T> to_quat(const rot_vec<T>& rotation_vector) {
    vector<T,3> unit_axis = rotation_vector.axis(); 
    T angle               = rotation_vector.angle();
    
    quat<T> output;
    output.setScalar(std::cos(angle/2.0));
    output.setVector(unit_axis * std::sin(angle/2.0));

    return output.normalize();
};

// Matrix can be either a rotation or transformation matrix 
// but the quaternion will not be the same
template<typename T>
quat<T> to_quat(const matrix<T,3,3>& matrix) {
    quat<T> output;
    const T numerical_limits = 100.0 * std::numeric_limits<T>::epsilon();

    T radicand = std::max(1.0 + matrix.trace(), 0.0); // trace of matrix should always be > -1
    output(3)  = 0.5 * std::sqrt(radicand);

    if (output(3) > numerical_limits) {
        T constant = 1.0 / (4.0*output(3));
        output(0)  = constant * (matrix(1,2) - matrix(2,1));
        output(1)  = constant * (matrix(2,0) - matrix(0,2));
        output(2)  = constant * (matrix(0,1) - matrix(1,0));

        return output.inv();
    }

    T sqrt_1 = std::sqrt(std::max(1.0 + matrix(0,0) - matrix(1,1) - matrix(2,2), 0.0));
    T sqrt_2 = std::sqrt(std::max(1.0 - matrix(0,0) + matrix(1,1) - matrix(2,2), 0.0));
    T sqrt_3 = std::sqrt(std::max(1.0 - matrix(0,0) - matrix(1,1) + matrix(2,2), 0.0));

    int largest_index = 0;
    if (sqrt_2 > sqrt_1 + numerical_limits)                   largest_index = 1;
    if (sqrt_3 > std::max(sqrt_1, sqrt_2) + numerical_limits) largest_index = 2;

    T constant = 0;

    switch (largest_index) {
        case 0:
            output(0)  = 0.5 * sqrt_1;
            
            constant = 1.0 / (4.0*output(0)); 
            output(1)  = constant * (matrix(0,1) + matrix(1,0));
            output(2)  = constant * (matrix(0,2) + matrix(2,0));
            output(3)  = constant * (matrix(1,2) - matrix(2,1));
            break;

        case 1:
            output(1)  = 0.5 * sqrt_2;
            
            constant = 1.0 / (4.0*output(1)); 
            output(0)  = constant * (matrix(1,0) + matrix(0,1));
            output(2)  = constant * (matrix(1,2) + matrix(2,1));
            output(3)  = constant * (matrix(2,0) - matrix(0,2));
            break;
            
        case 2:
            output(2)  = 0.5 * sqrt_3;
            
            constant = 1.0 / (4.0*output(2)); 
            output(0)  = constant * (matrix(2,0) + matrix(0,2));
            output(1)  = constant * (matrix(2,1) + matrix(1,2));
            output(2)  = constant * (matrix(0,1) - matrix(1,0));
            break;
    }

    return output.inv();     
};

#endif