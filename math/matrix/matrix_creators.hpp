#ifndef MATRIX_CREATORS_HPP
#define MATRIX_CREATORS_HPP

#include "matrix.hpp"

template <typename T, std::size_t N>
matrix<T, N, N> identity_matrix() {
    
    matrix<T, N, N> result;
    result.setIdentity();
    
    return result;
}


template <typename T, std::size_t rows>
matrix<T, rows, rows> diag_matrix(const matrix<T, rows, 1>& diag_values) {

    matrix<T, rows, rows> result; // <rows, rows> forces the returned matrix to be square
    result.setZeros();

    for (std::size_t i = 0; i < rows; i++) {
        result(i, i) = diag_values(i,0);
    }

    return result;
}

template <typename T, std::size_t N>
matrix<T, N, N> skew_matrix(const matrix<T, (N * (N - 1)) / 2, 1>& skew_vec) {
    matrix<T, N, N> result;
    result.set_skew(skew_vec);
    return result;
}

#endif