#ifndef MATRIX_ADVANCED_MATH_HPP
#define MATRIX_ADVANCED_MATH_HPP

#include "matrix.hpp"

template <typename T, std::size_t rows, std::size_t columns>
matrix<T, rows, columns> matrix<T, rows, columns>::elementWiseMultiply(const matrix<T, rows, columns>& right_hand_side) const {
    matrix<T, rows, columns> new_matrix{}; 

    for (std::size_t new_row = 0; new_row < rows; new_row++) {
        for (std::size_t new_column = 0; new_column < columns; new_column++) {
            new_matrix(new_row, new_column) = data[new_row][new_column] * right_hand_side(new_row, new_column); 
        }
    }

    return new_matrix;
}

template <typename T, std::size_t rows, std::size_t columns>
matrix<T, rows, columns> matrix<T, rows, columns>::elementWiseDivision(const matrix<T, rows, columns>& right_hand_side) const {
    matrix<T, rows, columns> new_matrix{}; 

    for (std::size_t new_row = 0; new_row < rows; new_row++) {
        for (std::size_t new_column = 0; new_column < columns; new_column++) {
            new_matrix(new_row, new_column) = data[new_row][new_column] / right_hand_side(new_row, new_column); 
        }
    }

    return new_matrix;
}

// Generic transpose must return another matrix if the transposed matrix is 
// not square because the outputted matrix would be of a different type (i.e. size) 
template <typename T, std::size_t rows, std::size_t columns>
matrix<T, columns, rows> matrix<T, rows, columns>::transpose() const {
    matrix<T, columns, rows> output;

    for (std::size_t row = 0; row < rows; row++) {
        for (std::size_t column = 0; column < columns; column++) {
            output(column, row) = data[row][column];
        }
    }

    return output;
}

// In-place transpose can simply swap the current rows and columns and the output is 
// the same type (i.e size)
template <typename T, std::size_t rows, std::size_t columns>
void matrix<T, rows, columns>::inPlaceTranspose() {
    static_assert(rows == columns, "In-place transpose only valid for square matrices.");

    for (std::size_t i = 0; i < rows; ++i) {
        for (std::size_t j = i + 1; j < columns; ++j) {
            std::swap(data[i][j], data[j][i]);
        }
    }
}

template <typename T, std::size_t rows, std::size_t columns>
T matrix<T, rows, columns>::det() const {
    static_assert(rows == columns, "Determinant only valid for square matrices.");
    static_assert(std::is_floating_point<T>::value, "Gaussian elimination in determinant function requires double/float matrix type");

    matrix<T, rows, columns> matrix_copy(*this);

    long double log_of_determinant = 0.0;
    int determinant_sign           = 1; 
    T determinant                  = static_cast<T>(0);
    std::array<T, rows> row_scale;

    // Step 1: Compute row scale factors to increase numerical precision
    for (std::size_t row = 0; row < rows; row++) {
        T row_max = static_cast<T>(0);

        for (std::size_t column = 0; column < columns; column++) {
            row_max = std::max(row_max, std::abs(matrix_copy(row, column)));
        }
        
        row_scale[row] = std::max(std::abs(row_max), std::numeric_limits<T>::epsilon());
    }
    
    
    for (std::size_t pivot_index = 0; pivot_index < rows - 1; pivot_index++) {                       
        // Step 2: Find best pivot row using scaled partial pivoting
        std::size_t best_row = pivot_index; 
        T largest_ratio      = std::abs(matrix_copy(pivot_index, pivot_index)) / row_scale[pivot_index];
        
        for (std::size_t current_row = pivot_index + 1; current_row < rows; current_row++) {
            T current_ratio = std::abs(matrix_copy(current_row, pivot_index)) / row_scale[current_row];

            if (current_ratio > largest_ratio) {
                largest_ratio = current_ratio; 
                best_row      = current_row; 
            }
        }

        // Step 3: Swap rows if needed current pivot row is not the best numerically
        if (best_row != pivot_index) {
            for (std::size_t i = 0; i < columns; i++) {
                std::swap(matrix_copy(pivot_index, i), matrix_copy(best_row, i));
            }
            std::swap(row_scale[pivot_index], row_scale[best_row]);
            determinant_sign = -determinant_sign;
        }

        // Step 4: Determinant is zero if pivot and remaining elements in the pivot column are 0.
        // Treat the pivot as zero if it is smaller than machine epsilon relative to its row scale
        T pivot_value = matrix_copy(pivot_index, pivot_index);
        if (std::abs(pivot_value) < row_scale[pivot_index] * std::numeric_limits<T>::epsilon()) {
            return determinant = static_cast<T>(0); 
        }

        // Step 5: Eliminate matrix cells below pivot
        for (std::size_t row = pivot_index + 1; row < rows; row++) {
            T factor = matrix_copy(row, pivot_index) / pivot_value;
            for (std::size_t column = pivot_index; column < columns; column++) {
                matrix_copy(row, column) -= factor * matrix_copy(pivot_index, column);
            }
        }
    }

    // Step 6: Sum the log of the diagonal elements to increase numerical precision 
    // then conver to non-log based determinant
    for (std::size_t i = 0; i < rows; i++) {
        if (matrix_copy(i,i) < 0) {
            determinant_sign = -determinant_sign;
        }
        log_of_determinant += std::log(std::abs(matrix_copy(i,i)));
    }
    
    return determinant = determinant_sign * std::exp(log_of_determinant);
} 

template <typename T, std::size_t rows, std::size_t columns>
matrix<T, rows, columns> matrix<T, rows, columns>::inv() const {
    static_assert(rows == columns, "Determinant only valid for square matrices.");
    static_assert(std::is_floating_point<T>::value, "Gaussian elimination in determinant function requires double/float matrix type");

    matrix<T, rows, columns> P;
    matrix<T, rows, columns> L;
    matrix<T, rows, columns> U(*this);
    matrix<T, rows, columns> inverse_matrix;

    P.setIdentity();
    L.setIdentity();
    inverse_matrix.setZeros();


    std::array<T, rows> row_scale; 

    // See det() for comments on gaussian elimination steps
    for (std::size_t row = 0; row < rows; row++) {
        T row_max = static_cast<T>(0);

        for (std::size_t column = 0; column < columns; column++) {
            row_max = std::max(row_max, std::abs(U(row, column)));
        }
        
        row_scale[row] = std::max(std::abs(row_max), std::numeric_limits<T>::epsilon());
    }

    
    for (std::size_t pivot_index = 0; pivot_index < rows - 1; pivot_index++) {                       
        std::size_t best_row = pivot_index; 
        T largest_ratio      = std::abs(U(pivot_index, pivot_index)) / row_scale[pivot_index];
        
        for (std::size_t current_row = pivot_index + 1; current_row < rows; current_row++) {
            T current_ratio = std::abs(U(current_row, pivot_index)) / row_scale[current_row];

            if (current_ratio > largest_ratio) {
                largest_ratio = current_ratio; 
                best_row      = current_row; 
            }
        }


        if (best_row != pivot_index) {
            for (std::size_t i = 0; i < columns; i++) {
                std::swap(P(pivot_index, i), P(best_row, i));
                std::swap(U(pivot_index, i), U(best_row, i));
            }

            for (std::size_t j = 0; j < pivot_index; j++) {
                std::swap(L(pivot_index, j), L(best_row, j));
            }
            
            std::swap(row_scale[pivot_index], row_scale[best_row]);
        }
        
        
        T pivot_value = U(pivot_index, pivot_index);

        if (std::abs(pivot_value) < std::numeric_limits<T>::epsilon() * row_scale[pivot_index]) {
            throw std::runtime_error("Matrix is singular to working precision");
        }

        for (std::size_t row = pivot_index + 1; row < rows; row++) {
            T factor = U(row, pivot_index) / pivot_value;
            for (std::size_t column = pivot_index; column < columns; column++) {
                U(row, column) -= factor * U(pivot_index, column);
            }

            L(row, pivot_index) = factor; 
        }
    }

    // Backwards and forwards substitution to solve for 
    for (std::size_t column = 0; column < columns; column++) {            
        
        // Forward substitution solve Ly = b where b = columns of P
        matrix<T, rows, 1> y;

        for (std::size_t row = 0; row < rows; row++) {
            y(row, 0) = P(row,column);

            for (std::size_t j = 0; j < row; j++) {
                y(row, 0) -= L(row, j) * y(j, 0);
            }
        }

        // Backwards substitution solve Ux = y
        matrix<T, rows, 1> x;

        for (int row = static_cast<int>(rows) - 1; row >= 0; row--) {
            x(row, 0) = y(row, 0);

            for (std::size_t j = row + 1; j < rows; j++) {
                x(row, 0) -= U(row, j) * x(j, 0);
            }

            x(row, 0) /= U(row,row);  
        }

        // Plug values into inverse 
        for (std::size_t r = 0; r < rows; r++) {
            inverse_matrix(r, column) = x(r, 0);
        }
    }

    return inverse_matrix;
}


// Matrix to a power uses exponentiation by squaring algorithm to improve efficiency
template <typename T, std::size_t rows, std::size_t columns>
matrix<T, rows, columns> matrix<T, rows, columns>::pow(int exponent) const {
    static_assert(rows == columns, "Matrix power only defined for square matrices.");

    matrix<T, rows, columns> new_matrix;
    matrix<T, rows, columns> matrix_copy(*this);
    new_matrix.setIdentity();

    if (exponent == 0) {
        return new_matrix;
    }
    
    if (exponent < 0) {
        matrix_copy = matrix_copy.inv();
        exponent    = -exponent;
    }
    
    
    bool exponent_is_odd;

    while (exponent > 0) {
        exponent_is_odd = (exponent % 2 == 1);

        if (exponent_is_odd) {
            new_matrix *= matrix_copy;
        }

        matrix_copy *= matrix_copy;
        exponent = exponent / 2;
    }       

    return new_matrix;
}


#endif