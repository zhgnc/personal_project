#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
#include <array>
#include <initializer_list>
#include <stdexcept>

template <typename T, std::size_t length>
class Vector 
{
public:
    std::array<T, length> vector{};
    std::size_t size = 0; 

    // Default constructor
    Vector() = default;

    // constructor if using initializer list
    Vector(std::initializer_list<T> initial_vector) {
        if (initial_vector.size() > length) {
            throw std::out_of_range("Too many elements in vector initialize list");
        }   
        
        std::size_t i = 0;
        for (T value : initial_vector) {
            vector[i] = value;
            i++; 
        }
    }

    T& operator[](std::size_t index) {
        return vector[index];
    }

    const T& operator[](std::size_t index) const {
        return vector[index];
    } 

    Vector operator*(const Vector& other_vector) const {
        Vector output_vector; 
        for (std::size_t i = 0; i < length; i++) {
            output_vector[i] = vector[i] * other_vector[i];
        }
    
        return output_vector;
    }

private: 


};


#endif