#ifndef QUAT_UTILITIES_HPP
#define QUAT_UTILITIES_HPP

#include <iostream>
#include "quat.hpp"
#include "../vector/vector.hpp"

template <typename type>
type& quat<type>::operator()(std::size_t element_to_return) {
    if (element_to_return >= 4) {
        throw std::out_of_range("Cannot return quaternion element greater than 4!");
    }

    return data[element_to_return];
}

template <typename type>
const type& quat<type>::operator()(std::size_t element_to_return) const {
    if (element_to_return >= 4) {
        throw std::out_of_range("Cannot return quaternion element greater than 4!");
    }

    return data[element_to_return];
}

template <typename type>
quat<type>& quat<type>::operator=(const quat<type>& another_quaternion) {
    data = another_quaternion.data;
    return (*this);
}

template <typename type>
quat<type>& quat<type>::operator=(const std::array<type,4>& std_array_quat) {
    data = std_array_quat;
    return (*this);
}

template <typename type>
void quat<type>::print() const {
    for (std::size_t row = 0; row < 4; row++) {
        std::cout << data[row] << "\n"; 
    }
    std::cout << "\n";
}

template <typename type>
void quat<type>::set_identity() {
    data = {0, 0, 0, 1};
}

template <typename type>
type quat<type>::scalar() const {
    return data[3];
}

template <typename type>
vector<type,3> quat<type>::vec() const {
    vector<type, 3> v;
    
    v(0) = data[0]; 
    v(1) = data[1];
    v(2) = data[2];
    
    return v;
}

template <typename type>
void quat<type>::setVector(vector<type, 3> vector) {
    data[0] = vector(0);
    data[1] = vector(1);
    data[2] = vector(2);
}

template <typename type>
void quat<type>::setScalar(type scalar) {
    data[3] = scalar;
}

template <typename type>
quat<type> quat<type>::neg() const{
    return quat<type>({-data[0], -data[1], -data[2], -data[3]});
}

template <typename type>
template <typename U>
quat<type>::operator quat<U>() const {
    quat<U> output;

    for (std::size_t row = 0; row < 4; row++) {
        output(row) = static_cast<U>(data[row]);
    }

    return output;
}

template <typename type>
quat<type>::operator std::array<type,4>() const {
    return data;
}

#endif