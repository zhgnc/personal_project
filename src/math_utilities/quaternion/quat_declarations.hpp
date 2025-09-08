#ifndef QUAT_DECLARATIONS_HPP
#define QUAT_DECLARATIONS_HPP

#include <array>
#include <iostream>
#include "quat.hpp"
#include "../vector/vector.hpp"

template <typename T>
class quat 
{
public:
    std::array<T, 4> data;

    // Constructors
    quat();
    quat(std::initializer_list<T> initial_quaternion);

    // Utilities
    T& operator()(std::size_t element_to_return);
    const T& operator()(std::size_t element_to_return) const;
    void print() const;
    void setIdentity();
    T scalar() const;
    vector<T,3> vec() const;
    void setVector(vector<T, 3> vector);
    void setScalar(T scalar);

    // Math
    quat<T> normalize() const;
    quat<T> conjugate() const;
    quat<T> inv() const;
    quat<T> operator*(quat<T> q_12) const;
    vector<T, 3> operator*(vector<T, 3> vector) const;
};

#endif