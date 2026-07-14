#ifndef MATRIX_CREATORS_HPP
#define MATRIX_CREATORS_HPP

#include "matrix.hpp"

template <typename type, std::size_t N>
matrix<type, N, N> identity_matrix() {
    
    matrix<type, N, N> result;
    result.set_identity();
    
    return result;
}


template <typename type, std::size_t rows>
matrix<type, rows, rows> diag_matrix(const matrix<type, rows, 1>& diag_values) {

    matrix<type, rows, rows> result; // <rows, rows> forces the returned matrix to be square
    result.set_zeros();

    for (std::size_t i = 0; i < rows; i++) {
        result(i, i) = diag_values(i,0);
    }

    return result;
}

template <typename type, std::size_t N>
matrix<type, N, N> skew_matrix(const matrix<type, (N * (N - 1)) / 2, 1>& skew_vec) {
    matrix<type, N, N> result;
    result.set_skew(skew_vec);
    return result;
}

#endif