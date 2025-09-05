#ifndef VECTOR_HPP
#define VECTOR_HPP

#include "matrix.hpp"
#include <algorithm> // std::clamp
#include <cmath>

template <typename T, std::size_t length>
class vector : public matrix<T, length, 1> 
{
public:
    using matrix<T, length, 1>::matrix; // Inherit base constructors

    // Allows a matrix with 1 column to initialize a vector object.
    // For example this is now valid syntax: vector = matrix * vector;
    vector(const matrix<T, length, 1>& mat) : matrix<T, length, 1>(mat) {}; 

    T& operator()(std::size_t row_to_return) {
        return matrix<T,length,1>::data[row_to_return][0];
    }

    const T& operator()(std::size_t row_to_return) const {
        return matrix<T,length,1>::data[row_to_return][0];
    }

    T dot(const vector<T, length>& v2) const {
        T output = 0;
        
        for (std::size_t row = 0; row < length; row++) {
            output += (*this)(row) * v2(row);
        }

        return output;
    }

    vector<T, length> cross(const vector<T,length>& v2) const {
        static_assert(length == 3, "Cross product only defined for 3D vectors");
        
        vector<T,3> output;
        
        output(0) = (*this)(1) * v2(2) - (*this)(2) * v2(1);
        output(1) = (*this)(2) * v2(0) - (*this)(0) * v2(2);
        output(2) = (*this)(0) * v2(1) - (*this)(1) * v2(0);
        
        return output;
    }

    T mag() const {
        T output = 0;

        for (std::size_t row = 0; row < length; row++) {
            output += (*this)(row) * (*this)(row);
        }

        return std::sqrt(output);
    }

    vector<T, length> norm() const {
        T magnitude = mag();
        if (magnitude == 0) throw std::runtime_error("Cannot normalize vector with all elements zero!");

        vector<T,length> output;
        for (std::size_t row = 0; row < length; row++) {
            output(row) = (*this)(row) / mag;
        }

        return output;
    }

    T angle_between(const vector<T,length>& v2) const {
        T magnitude_1 = mag();
        T magnitude_2 = v2.mag();

        if (magnitude_1 < 1e-12 || magnitude_2 < 1e-12) {
            throw std::runtime_error("Cannot comput angle with zero length vector!");
        }

        T temp = (*this).dot(v2) / (magnitude_1 * magnitude_2);
        temp = std::clamp(temp, T(-1), T(1));

        T angle_in_radians = std::acos(temp);


        return angle_in_radians;
    }

private: 


};


#endif