#ifndef MATRIX_UTILITIES_HPP
#define MATRIX_UTILITIES_HPP

#include "matrix.hpp"

template <typename T, std::size_t rows, std::size_t columns>
T& matrix<T, rows, columns>::operator()(std::size_t desired_row, std::size_t desired_column) {
    if (desired_row >= rows || desired_column >= columns) {
        throw std::out_of_range("Requested element outside of row and column bounds.");
    }  

    return data[desired_row][desired_column];
}

template <typename T, std::size_t rows, std::size_t columns>
const T& matrix<T, rows, columns>::operator()(std::size_t desired_row, std::size_t desired_column) const {
    if (desired_row >= rows || desired_column >= columns) {
        throw std::out_of_range("Requested element outside of row and column bounds.");
    }

    return data[desired_row][desired_column];
}

template <typename T, std::size_t rows, std::size_t columns>
void matrix<T, rows, columns>::print() const {
    for (std::size_t row = 0; row < rows; row++) {
        for (std::size_t column = 0; column < columns; column++) {
            std::cout << data[row][column] << "   "; 
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

template <typename T, std::size_t rows, std::size_t columns>
void matrix<T, rows, columns>::set_row(std::size_t row_index, std::initializer_list<T> row_values) {
    if (row_index >= rows || row_values.size() != columns) {
        throw std::invalid_argument("Row index out of range or initializer list size mismatch");
    } 

    std::size_t new_column = 0;

    for (const T& value : row_values) {
        data[row_index][new_column++] = value;
    }
}

template <typename T, std::size_t rows, std::size_t columns>
void matrix<T, rows, columns>::set_column(std::size_t column_index, std::initializer_list<T> column_values) {
    if (column_index >= columns || column_values.size() != rows) {
        throw std::invalid_argument("Column index out of range or initializer list size mismatch");
    }

    std::size_t new_row = 0;

    for (const T& value : column_values) {
        data[new_row++][column_index] = value;
    }
}

template <typename T, std::size_t rows, std::size_t columns>
void matrix<T, rows, columns>::setZeros() {
    for (std::size_t row = 0; row < rows; row++) {
        for (std::size_t column = 0; column < columns; column++) {
            data[row][column] = T{0};
        }
    }
}

template <typename T, std::size_t rows, std::size_t columns>
void matrix<T, rows, columns>::setIdentity() {
    static_assert(rows == columns, "Identity requires square matrix.");
    setZeros();

    for (std::size_t diag_index = 0; diag_index < rows; diag_index++) {
        data[diag_index][diag_index] = T{1};
    }
}

// Factory functions like zerosMatrix and identityMatrix are defined outside the class
// because they create and return new matrix instances without needing an existing object.
// Defining them as free functions enables clean syntax and avoids coupling these utilities
// to any particular instance of the class.
template<typename T, std::size_t square_matrix_size>
matrix<T, square_matrix_size, square_matrix_size> identityMatrix() {
    matrix<T, square_matrix_size, square_matrix_size> identityMat;
    identityMat.setIdentity();
    return identityMat;
}

template<typename T, std::size_t num_rows, std::size_t num_columns>
matrix<T, num_rows, num_columns> zerosMatrix() {
    matrix<T, num_rows, num_columns> zerosMat;
    zerosMat.setZeros();
    return zerosMat;
}

template <typename T, std::size_t rows, std::size_t columns>
T matrix<T, rows, columns>::trace() const {
    static_assert(rows == columns, "Trace function requires square matrix.");

    T output = 0; 
    for (std::size_t i = 0; i < rows; i++) {
        output += data[i][i];
    }

    return output; 
}

template <typename T, std::size_t rows, std::size_t columns>
matrix<T, rows, 1> matrix<T, rows, columns>::get_diag() const {
    static_assert(rows == columns, "get_diag() only valid for square matrices");

    matrix<T, rows, 1> result;

    for (std::size_t i = 0; i < rows; i++) {
        result(i,0) = data[i][i];
    }

    return result;
}

template <typename T, std::size_t rows, std::size_t columns>
void matrix<T, rows, columns>::set_diag(const matrix<T, rows, 1>& diag_values) {
    static_assert(rows == columns, "set_diag requires square matrix");

    for (std::size_t i = 0; i < rows; i++) {
        data[i][i] = diag_values(i,0);
    }
}


template <typename T, std::size_t rows, std::size_t columns>
template <std::size_t block_rows, std::size_t block_columns>
void matrix<T, rows, columns>::set_block(std::size_t starting_row, std::size_t starting_column, const matrix<T, block_rows, block_columns>& block_to_insert) {

    assert(starting_row    + block_rows    <= rows); 
    assert(starting_column + block_columns <= columns);

    for (std::size_t i = 0; i < block_rows; i++) {
        for (std::size_t j = 0; j < block_columns; j++) {
            data[starting_row + i][starting_column + j] = block_to_insert(i,j);
        }
    }
}

template <typename T, std::size_t rows, std::size_t columns>
template <std::size_t block_rows, std::size_t block_columns>
matrix<T, block_rows, block_columns> matrix<T, rows, columns>::get_block(std::size_t starting_row, std::size_t starting_column) const {
    
    assert(starting_row    + block_rows    <= rows);
    assert(starting_column + block_columns <= columns);

    matrix<T, block_rows, block_columns> result;

    for (std::size_t i = 0; i < block_rows; ++i)
    {
        for (std::size_t j = 0; j < block_columns; ++j)
        {
            result(i, j) = data[starting_row + i][starting_column + j];
        }
    }

    return result;
}

template <typename T, std::size_t rows, std::size_t columns>
template <typename U>
matrix<T, rows,columns>::operator matrix<U, rows, columns>() const {
    matrix<U, rows,columns> output;

    for (std::size_t row = 0; row < rows; row++) {
        for (std::size_t column = 0; column < columns; column++) {
            output(row, column) = static_cast<U>(data[row][column]);
        }
    }

    return output;
}


#endif