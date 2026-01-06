#ifndef QUAT_UTILITIES_HPP
#define QUAT_UTILITIES_HPP

#include <iostream>
#include "quat.hpp"
#include "../vector/vector.hpp"

template <typename T>
T& quat<T>::operator()(std::size_t element_to_return) {
    if (element_to_return >= 4) {
        throw std::out_of_range("Cannot return quaternion element greater than 4!");
    }

    return data[element_to_return];
}

template <typename T>
const T& quat<T>::operator()(std::size_t element_to_return) const {
    if (element_to_return >= 4) {
        throw std::out_of_range("Cannot return quaternion element greater than 4!");
    }

    return data[element_to_return];
}

template <typename T>
quat<T>& quat<T>::operator=(const quat<T>& another_quaternion) {
    data = another_quaternion.data;
    return (*this);
}

template <typename T>
void quat<T>::print() const {
    for (std::size_t row = 0; row < 4; row++) {
        std::cout << data[row] << "\n"; 
    }
    std::cout << "\n";
}

template <typename T>
void quat<T>::setIdentity() {
    data = {0, 0, 0, 1};
}

template <typename T>
T quat<T>::scalar() const {
    return data[3];
}

template <typename T>
vector<T,3> quat<T>::vec() const {
    vector<T, 3> v;
    
    v(0) = data[0]; 
    v(1) = data[1];
    v(2) = data[2];
    
    return v;
}

template <typename T>
void quat<T>::setVector(vector<T, 3> vector) {
    data[0] = vector(0);
    data[1] = vector(1);
    data[2] = vector(2);
}

template <typename T>
void quat<T>::setScalar(T scalar) {
    data[3] = scalar;
}

template <typename T>
quat<T> quat<T>::neg() const{
    return quat<T>({-data[0], -data[1], -data[2], -data[3]});
}

template <typename T>
template <typename U>
quat<T>::operator quat<U>() const {
    quat<U> output;

    for (std::size_t row = 0; row < 4; row++) {
        output(row) = static_cast<U>(data[row]);
    }

    return output;
}

#endif