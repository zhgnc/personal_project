#ifndef MATRIX_SIMPLE_MATH_HPP
#define MATRIX_SIMPLE_MATH_HPP

#include "matrix.hpp"

// A = B * C
template <typename T, std::size_t rows, std::size_t columns>
template <std::size_t other_columns>
matrix<T, rows, other_columns> matrix<T, rows, columns>::operator*(const matrix<T, columns, other_columns>& right_hand_side) const {
    matrix<T, rows, other_columns> new_matrix; 
    
    for (std::size_t new_row = 0; new_row < rows; new_row++) {
        for (std::size_t new_column = 0; new_column < other_columns; new_column++) {
            for (std::size_t shared_dimension = 0; shared_dimension < columns; shared_dimension++) {
                new_matrix(new_row, new_column) += data[new_row][shared_dimension] * right_hand_side(shared_dimension, new_column);
            }
        }
    }
    
    return new_matrix; 
}

// A = B * scalar
template <typename T, std::size_t rows, std::size_t columns>
matrix<T, rows, columns> matrix<T, rows, columns>::operator*(const T& scalar) const {
    matrix<T, rows, columns> new_matrix; 
    
    for (std::size_t new_row = 0; new_row < rows; new_row++) {
        for (std::size_t new_column = 0; new_column < columns; new_column++) {
            new_matrix(new_row, new_column) = scalar * data[new_row][new_column];
        }
    }
    
    return new_matrix; 
}

template <typename T, std::size_t rows, std::size_t columns>
matrix<T, rows, columns>& matrix<T, rows, columns>::operator*=(const matrix<T, rows, columns>& rhs) {
    static_assert(rows == columns, "In-place multiplication only valid for square matrices.");
    
    *this = (*this) * rhs;
    return *this;
}

// A = B + C
template <typename T, std::size_t rows, std::size_t columns>
matrix<T, rows, columns> matrix<T, rows, columns>::operator+(const matrix<T, rows, columns>& right_hand_side) const {
    matrix<T, rows, columns> new_matrix; 

    for (std::size_t new_row = 0; new_row < rows; new_row++) {
        for (std::size_t new_column = 0; new_column < columns; new_column++) {
            new_matrix(new_row, new_column) = data[new_row][new_column] + right_hand_side(new_row, new_column); 
        }
    }

    return new_matrix;
}

// A = B + scalar
template <typename T, std::size_t rows, std::size_t columns>
matrix<T, rows, columns> matrix<T, rows, columns>::operator+(const T& scalar) const {
    matrix<T, rows, columns> new_matrix; 

    for (std::size_t new_row = 0; new_row < rows; new_row++) {
        for (std::size_t new_column = 0; new_column < columns; new_column++) {
            new_matrix(new_row, new_column) = data[new_row][new_column] + scalar; 
        }
    }

    return new_matrix;
}

// A = B - C
template <typename T, std::size_t rows, std::size_t columns>
matrix<T, rows, columns> matrix<T, rows, columns>::operator-(const matrix<T, rows, columns>& right_hand_side) const {
    matrix<T, rows, columns> new_matrix; 

    for (std::size_t new_row = 0; new_row < rows; new_row++) {
        for (std::size_t new_column = 0; new_column < columns; new_column++) {
            new_matrix(new_row, new_column) = data[new_row][new_column] - right_hand_side(new_row, new_column); 
        }
    }

    return new_matrix;
}

// A = B - scalar
template <typename T, std::size_t rows, std::size_t columns>
matrix<T, rows, columns> matrix<T, rows, columns>::operator-(const T& scalar) const {
    matrix<T, rows, columns> new_matrix; 

    for (std::size_t new_row = 0; new_row < rows; new_row++) {
        for (std::size_t new_column = 0; new_column < columns; new_column++) {
            new_matrix(new_row, new_column) = data[new_row][new_column] - scalar; 
        }
    }

    return new_matrix;
}

// A = -B
template <typename T, std::size_t rows, std::size_t columns>
matrix<T, rows, columns> matrix<T, rows, columns>::operator-() const {
    matrix<T, rows, columns> new_matrix; 

    for (std::size_t new_row = 0; new_row < rows; new_row++) {
        for (std::size_t new_column = 0; new_column < columns; new_column++) {
            new_matrix(new_row, new_column) = -data[new_row][new_column]; 
        }
    }

    return new_matrix;
}

// A = B / scalar
template <typename T, std::size_t rows, std::size_t columns>
matrix<T, rows, columns> matrix<T, rows, columns>::operator/(const T& scalar) const {
    matrix<T, rows, columns> new_matrix; 

    for (std::size_t new_row = 0; new_row < rows; new_row++) {
        for (std::size_t new_column = 0; new_column < columns; new_column++) {
            new_matrix(new_row, new_column) = data[new_row][new_column] / scalar; 
        }
    }

    return new_matrix;
}

// Enables scalar * matrix (e.g. 5 * matrix), which not supported by member functions.
// Member functions can only handle matrix * scalar because they cannot accept two input arguments.
// That is why these last two functions do not have the same scope as the others
template<typename T, std::size_t rows, std::size_t columns>
matrix<T, rows, columns> operator*(const T& scalar, const matrix<T, rows, columns>& matrix) {
    return matrix * scalar;
}

// A = scalar + B
template<typename T, std::size_t rows, std::size_t columns>
matrix<T, rows, columns> operator+(const T& scalar, const matrix<T, rows, columns>& matrix) {
    return matrix + scalar;
}


#endif