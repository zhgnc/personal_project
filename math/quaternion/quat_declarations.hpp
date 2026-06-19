#ifndef QUAT_DECLARATIONS_HPP
#define QUAT_DECLARATIONS_HPP

#include <array>
#include <iostream>
#include "quat.hpp"
#include "../vector/vector.hpp"

template <typename type>
class quat 
{
public:
    std::array<type, 4> data;

    // Constructors
    quat();
    quat(std::initializer_list<type> initial_quaternion);
    quat(const quat<type>& another_quaternion);
    explicit quat(const std::array<type,4>& std_array_quat);

    // Utilities
    type& operator()(std::size_t element_to_return);
    const type& operator()(std::size_t element_to_return) const;
    quat<type>& operator=(const quat<type>& another_quaternion);
    quat<type>& operator=(const std::array<type,4>& std_array_quat);

    void print() const;
    void set_identity();
    type scalar() const;
    vector<type,3> vec() const;
    void setVector(vector<type, 3> vector);
    void setScalar(type scalar);
    quat<type> neg() const;

    template <typename U>
    explicit operator quat<U>() const;
    operator std::array<type,4>() const;

    // Math
    quat<type> normalize() const;
    quat<type> conjugate() const;
    quat<type> inv() const;
    quat<type> operator*(quat<type> q_12) const;
    vector<type, 3> operator*(vector<type, 3> vector) const;
};

#endif