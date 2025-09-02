#ifndef MATRIX_CONSTRUCTORS_HPP
#define MATRIX_CONSTRUCTORS_HPP


#include <iostream>
#include "matrix.hpp"

template <typename T, std::size_t rows, std::size_t columns>
matrix<T, rows, columns>::matrix() {
    setZeros();
}; 

template <typename T, std::size_t rows, std::size_t columns>
matrix<T, rows, columns>::matrix(std::initializer_list<T> initial_matrix) {
    if (initial_matrix.size() != rows * columns) {
        throw std::out_of_range("Size of matrix does not equal size of initializer list");
    }        

    std::size_t row    = 0; 
    std::size_t column = 0;
    for (const T& element : initial_matrix) {
        data[row][column] = element; 
        column = column + 1; 
        if (column == columns) {
            column = 0; 
            row = row + 1; 
        }
    }
}

#endif

