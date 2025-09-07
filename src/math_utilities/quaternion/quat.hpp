#ifndef QUAT_HPP
#define QUAT_HPP

#include <array>
#include <iostream>
#include "../vector/vector.hpp"

template <typename T>
class quat 
{
public:
    std::array<T, 4> data;

    quat() {
        setIdentity();
    }

    quat(std::initializer_list<T> initial_quaternion) {
        if (initial_quaternion.size() != 4) {
            throw std::out_of_range("Quaternion must have 4 element when initializing/assigning data!");
        }   
        
        std::size_t row = 0;
        for (const T& element : initial_quaternion) {
            data[row] = element; 
            row += 1; 
        }
    }

    T& operator()(std::size_t element_to_return) {
        if (element_to_return >= 4) {
            throw std::out_of_range("Cannot return quaternion element greater than 4!");
        }

        return data[element_to_return];
    }

    const T& operator()(std::size_t element_to_return) const {
        if (element_to_return >= 4) {
            throw std::out_of_range("Cannot return quaternion element greater than 4!");
        }

        return data[element_to_return];
    }

    void print() const {
        for (std::size_t row = 0; row < 4; row++) {
            std::cout << data[row] << "\n"; 
        }
        std::cout << "\n";
    }
    
    void setIdentity() {
        data = {0, 0, 0, 1};
    }

    void norm() {
        T magnitude = 0.0; 
        
        for (std::size_t row = 0; row < 4; row++) {
            magnitude += data[row]*data[row];
        }

        for (std::size_t row = 0; row < 4; row++) {
            data[row] = data[row] / magnitude;
        }
    }

    T scalar() const {
        return data[3];
    }

    vector<T,3> vec() {
        vector<T, 3> v;
        
        v(0) = data[0]; 
        v(1) = data[1];
        v(2) = data[2];
        
        return v;
    }

    // Quaternion multiplication is in same order as matrixes
    // q_1_to_3 = q_2_to_3 * q_1_to_2
    // Note the '_to_' is removed in the variable names below
    quat<T> operator*(quat<T> q_12) const {
        quat<T> q_23 = (*this); 
        quat<T> q_13; 

        T s12            = q_12.scalar(); 
        T s23            = q_23.scalar();
        vector<T, 3> v12 = q_12.vec();
        vector<T, 3> v23 = q_23.vec();

        q_13.setVector(s12*v23 + s23*v12 - v12.cross(v23));
        q_13.setScalar(s12*s23 - v12.dot(v23));

        return q_13;
    }

    void setVector(vector<T, 3> vector) {
        data[0] = vector(0);
        data[1] = vector(1);
        data[2] = vector(2);
    }

    void setScalar(T scalar) {
        data[3] = scalar;
    }

    quat<T> conjugate() const {
        quat<T> output = {-data[0], 
                          -data[1], 
                          -data[2], 
                           data[3]};

        
        return output;
    }
    
    vector<T, 3> operator*(vector<T, 3> vector) const {
        quat<T> q_from_vector;
        q_from_vector.setVector(vector);

        quat<T> q_new = (*this) * q_from_vector * (*this).conjugate();
        
        return {q_new(0), q_new(1), q_new(2)};
    }
};

#endif