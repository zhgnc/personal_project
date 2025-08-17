#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>

template <typename T, std::size_t rows, std::size_t columns>
class Matrix2D 
{
public:
    static_assert(std::is_arithmetic_v<T>, "Matrix2D requires numeric types.");
    std::array<std::array<T, columns>, rows> matrix;
    static constexpr std::size_t num_rows    = rows;
    static constexpr std::size_t num_columns = columns;
    static constexpr std::size_t size = rows * columns;
    
    Matrix2D() = default; 

    Matrix2D(std::initializer_list<T> initial_matrix) {
        if (rows * columns != initial_matrix.size()) {
            throw std::out_of_range("Size of matrix does not equal size of initializer list");
        }   
        
        std::size_t row    = 0; 
        std::size_t column = 0;
        for (const T& element : initial_matrix) {
            matrix[row][column] = element; 
            column = column + 1; 
            if (column == columns) {
                column = 0; 
                row = row + 1; 
            }
        }
    }

    void print() const {
        for (std::size_t row = 0; row < rows; row++) {
            for (std::size_t column = 0; column < columns; column++) {
                std::cout << matrix[row][column] << "   "; 
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    T& operator()(std::size_t desired_row, std::size_t desired_column) {
        return matrix[desired_row][desired_column];
    }

    const T& operator()(std::size_t desired_row, std::size_t desired_column) const {
        return matrix[desired_row][desired_column];
    }

    template <std::size_t other_columns>
    Matrix2D<T, rows, other_columns> operator*(const Matrix2D<T, columns, other_columns>& right_hand_side) const {
       
        const Matrix2D<T, rows, columns>& left_hand_side = *this; 
        Matrix2D<T, rows, other_columns> new_matrix{}; 
        
        for (std::size_t new_row = 0; new_row < rows; new_row++) {
            for (std::size_t new_column = 0; new_column < other_columns; new_column++) {
                for (std::size_t shared_dimension = 0; shared_dimension < columns; shared_dimension++) {
                    new_matrix(new_row, new_column) += left_hand_side(new_row, shared_dimension) * right_hand_side(shared_dimension, new_column);
                }
            }
        }
        
        return new_matrix; 
    }

    
    
    




private:


};


#endif
