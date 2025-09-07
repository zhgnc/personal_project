#ifndef QUAT_HPP
#define QUAT_HPP

#include <array>
#include <iostream>

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

        norm();
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
};

#endif