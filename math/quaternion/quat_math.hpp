#ifndef QUAT_MATH_HPP
#define QUAT_MATH_HPP

#include "quat.hpp"
#include "../vector/vector.hpp"

template <typename type>
quat<type> quat<type>::normalize() const {
    type sum_of_squares = 0.0; 
    quat<type> q_copy   = (*this);
    quat<type> output; 
    
    for (std::size_t row = 0; row < 4; row++) {
        sum_of_squares += q_copy(row)*q_copy(row);
    }

    type magnitude = std::sqrt(sum_of_squares);

    if (magnitude < 10.0 * std::numeric_limits<type>::epsilon()) {
        throw std::runtime_error("Cannot normalize a zero-magnitude quaternion!");
    }

    for (std::size_t row = 0; row < 4; row++) {
        output(row) = q_copy(row) / magnitude;
    }

    return output;
}

// Quaternion multiplication is in same order as matrixes
// q_1_to_3 = q_2_to_3 * q_1_to_2
// Note the '_to_' is removed in the variable names below
template <typename type>
quat<type> quat<type>::operator*(quat<type> q_12) const {
    quat<type> q_23 = (*this); 
    quat<type> q_13; 

    type s12            = q_12.scalar(); 
    type s23            = q_23.scalar();
    vector<type, 3> v12 = q_12.vec();
    vector<type, 3> v23 = q_23.vec();

    q_13.setVector(s12*v23 + s23*v12 + v23.cross(v12));
    q_13.setScalar(s12*s23 - v23.dot(v12));

    return q_13;
}

template <typename type>
quat<type> quat<type>::conjugate() const {
    return {-data[0], -data[1], -data[2], data[3]};
}

template <typename type>
quat<type> quat<type>::inv() const {
    return conjugate().normalize(); 
}

template <typename type>
vector<type, 3> quat<type>::operator*(vector<type, 3> vector) const {
    quat<type> q_from_vector = {0.0, 0.0, 0.0, 0.0};
    q_from_vector.setVector(vector);

    quat<type> q_new = (*this) * (q_from_vector * (*this).inv());
    
    return {q_new(0), q_new(1), q_new(2)};
}

#endif