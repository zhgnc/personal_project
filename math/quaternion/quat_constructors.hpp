#ifndef QUAT_CONSTRUCTORS_HPP
#define QUAT_CONSTRUCTORS_HPP

#include "quat.hpp"

template <typename T>
quat<T>::quat() {
    setIdentity();
}

template <typename T>
quat<T>::quat(std::initializer_list<T> initial_quaternion) {
    if (initial_quaternion.size() != 4) {
        throw std::out_of_range("Quaternion must have 4 element when initializing/assigning data!");
    }   
    
    std::size_t row = 0;
    for (const T& element : initial_quaternion) {
        data[row] = element; 
        row += 1; 
    }
}

template <typename T>
quat<T>::quat(const quat<T>& another_quaternion) {    
    data = another_quaternion.data;
}

template <typename T>
quat<T>::quat(const std::array<T,4>& std_array_quat) {
    for (std::size_t i = 0; i < 4; i++) {
        data[i] = std_array_quat[i];
    }
}

#endif