#ifndef QUAT_CONSTRUCTORS_HPP
#define QUAT_CONSTRUCTORS_HPP

#include "quat.hpp"

template <typename type>
quat<type>::quat() {
    set_identity();
}

template <typename type>
quat<type>::quat(std::initializer_list<type> initial_quaternion) {
    if (initial_quaternion.size() != 4) {
        throw std::out_of_range("Quaternion must have 4 element when initializing/assigning data!");
    }   
    
    std::size_t row = 0;
    for (const type& element : initial_quaternion) {
        data[row] = element; 
        row += 1; 
    }
}

template <typename type>
quat<type>::quat(const quat<type>& another_quaternion) {    
    data = another_quaternion.data;
}

template <typename type>
quat<type>::quat(const std::array<type,4>& std_array_quat) {
    for (std::size_t i = 0; i < 4; i++) {
        data[i] = std_array_quat[i];
    }
}

#endif