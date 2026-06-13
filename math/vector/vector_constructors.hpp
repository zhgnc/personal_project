#ifndef VECTOR_CONSTRUCTORS_HPP
#define VECTOR_CONSTRUCTORS_HPP

#include "vector.hpp"

template <typename T, std::size_t length>
vector<T, length>::vector(const matrix<T, length, 1>& mat) {
    for (std::size_t row = 0; row < length; row++) {
        (*this)(row) = mat(row,0);
    }
}; 

#endif