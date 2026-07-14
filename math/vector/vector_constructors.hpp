#ifndef VECTOR_CONSTRUCTORS_HPP
#define VECTOR_CONSTRUCTORS_HPP

#include "vector.hpp"

template <typename type, std::size_t length>
vector<type, length>::vector(const matrix<type, length, 1>& mat) {
    for (std::size_t row = 0; row < length; row++) {
        (*this)(row) = mat(row,0);
    }
}; 

#endif