#ifndef VECTOR_SIMPLE_MATH_HPP
#define VECTOR_SIMPLE_MATH_HPP

#include "vector.hpp"
#include <algorithm> // std::clamp
#include <cmath>


template <typename type, std::size_t length>
type vector<type,length>::dot(const vector<type, length>& v2) const {
    type output = 0;
    
    for (std::size_t row = 0; row < length; row++) {
        output += (*this)(row) * v2(row);
    }

    return output;
}

template <typename type, std::size_t length>
vector<type, length> vector<type,length>::cross(const vector<type,length>& v2) const {
    static_assert(length == 3, "Cross product only defined for 3D vectors");
    
    vector<type,3> output;
    
    output(0) = (*this)(1) * v2(2) - (*this)(2) * v2(1);
    output(1) = (*this)(2) * v2(0) - (*this)(0) * v2(2);
    output(2) = (*this)(0) * v2(1) - (*this)(1) * v2(0);
    
    return output;
}


template <typename type, std::size_t length>
type vector<type,length>::mag() const {
    type output = 0;

    for (std::size_t row = 0; row < length; row++) {
        output += (*this)(row) * (*this)(row);
    }

    return std::sqrt(output);
}

template <typename type, std::size_t length>
vector<type, length> vector<type,length>::norm() const {
    type magnitude = mag();
    if (magnitude == 0) {
        return {0.0, 0.0, 0.0};
    };

    vector<type,length> output;
    for (std::size_t row = 0; row < length; row++) {
        output(row) = (*this)(row) / magnitude;
    }

    return output;
}

template <typename type, std::size_t length>
type vector<type,length>::angle_between(const vector<type,length>& v2) const {
    type magnitude_1 = mag();
    type magnitude_2 = v2.mag();

    if (magnitude_1 < 1e-12 || magnitude_2 < 1e-12) {
        throw std::runtime_error("Cannot comput angle with zero length vector!");
    }

    type temp = (*this).dot(v2) / (magnitude_1 * magnitude_2);
    temp = std::clamp(temp, type(-1), type(1));

    type angle_in_radians = std::acos(temp);


    return angle_in_radians;
}

#endif