#ifndef QUAT_MATH_HPP
#define QUAT_MATH_HPP

#include "quat.hpp"
#include "../vector/vector.hpp"

template <typename T>
quat<T> quat<T>::normalize() const {
    T sum_of_squares = 0.0; 
    quat<T> q_copy   = (*this);
    quat<T> output; 
    
    for (std::size_t row = 0; row < 4; row++) {
        sum_of_squares += q_copy(row)*q_copy(row);
    }

    T magnitude = std::sqrt(sum_of_squares);

    for (std::size_t row = 0; row < 4; row++) {
        output(row) = q_copy(row) / magnitude;
    }

    return output;
}

// Quaternion multiplication is in same order as matrixes
// q_1_to_3 = q_2_to_3 * q_1_to_2
// Note the '_to_' is removed in the variable names below
template <typename T>
quat<T> quat<T>::operator*(quat<T> q_12) const {
    quat<T> q_23 = (*this); 
    quat<T> q_13; 

    T s12            = q_12.scalar(); 
    T s23            = q_23.scalar();
    vector<T, 3> v12 = q_12.vec();
    vector<T, 3> v23 = q_23.vec();

    q_13.setVector(s12*v23 + s23*v12 + v23.cross(v12));
    q_13.setScalar(s12*s23 - v23.dot(v12));

    return q_13;
}

template <typename T>
quat<T> quat<T>::conjugate() const {
    return {-data[0], -data[1], -data[2], data[3]};
}

template <typename T>
quat<T> quat<T>::inv() const {
    return conjugate().normalize(); 
}

template <typename T>
vector<T, 3> quat<T>::operator*(vector<T, 3> vector) const {
    quat<T> q_from_vector = {0.0, 0.0, 0.0, 0.0};
    q_from_vector.setVector(vector);

    quat<T> q_new = (*this) * (q_from_vector * (*this).inv());
    
    return {q_new(0), q_new(1), q_new(2)};
}

#endif