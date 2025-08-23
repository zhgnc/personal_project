#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <array>

template <typename T, std::size_t rows, std::size_t columns>
class Matrix2D 
{
public:
    static_assert(std::is_arithmetic_v<T>, "Matrix2D requires numeric types.");
    std::array<std::array<T, columns>, rows> matrix;
    static constexpr std::size_t num_rows    = rows;
    static constexpr std::size_t num_columns = columns;
    static constexpr std::size_t size        = rows * columns;
    
    Matrix2D() {
        setZeros();
    }; 

    Matrix2D(std::initializer_list<T> initial_matrix) {
        if (initial_matrix.size() != rows * columns) {
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

    T& operator()(std::size_t desired_row, std::size_t desired_column) {
        if (desired_row >= rows || desired_column >= columns) {
            throw std::out_of_range("Requested element outside of row and column bounds.");
        }  

        return matrix[desired_row][desired_column];
    }

    const T& operator()(std::size_t desired_row, std::size_t desired_column) const {
        if (desired_row >= rows || desired_column >= columns) {
            throw std::out_of_range("Requested element outside of row and column bounds.");
        }

        return matrix[desired_row][desired_column];
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

    void set_row(std::size_t row_index, std::initializer_list<T> row_values) {
        if (row_index >= rows || row_values.size() != columns) {
            throw std::invalid_argument("Row index out of range or initializer list size mismatch");
        } 

        std::size_t new_column = 0;

        for (const T& value : row_values) {
            matrix[row_index][new_column++] = value;
        }
    }

    void set_column(std::size_t column_index, std::initializer_list<T> column_values) {
        if (column_index >= columns || column_values.size() != rows) {
            throw std::invalid_argument("Column index out of range or initializer list size mismatch");
        }

        std::size_t new_row = 0;

        for (const T& value : column_values) {
            matrix[new_row++][column_index] = value;
        }
    }

    // A = B * C
    template <std::size_t other_columns>
    Matrix2D<T, rows, other_columns> operator*(const Matrix2D<T, columns, other_columns>& right_hand_side) const {
        Matrix2D<T, rows, other_columns> new_matrix{}; 
        
        for (std::size_t new_row = 0; new_row < rows; new_row++) {
            for (std::size_t new_column = 0; new_column < other_columns; new_column++) {
                for (std::size_t shared_dimension = 0; shared_dimension < columns; shared_dimension++) {
                    new_matrix(new_row, new_column) += matrix[new_row][shared_dimension] * right_hand_side(shared_dimension, new_column);
                }
            }
        }
        
        return new_matrix; 
    }

    // A = B * scalar
    Matrix2D<T, rows, columns> operator*(const T& scalar) const {
        Matrix2D<T, rows, columns> new_matrix{}; 
        
        for (std::size_t new_row = 0; new_row < rows; new_row++) {
            for (std::size_t new_column = 0; new_column < columns; new_column++) {
                new_matrix(new_row, new_column) = scalar * matrix[new_row][new_column];
            }
        }
        
        return new_matrix; 
    }

    // A = B + C
    Matrix2D<T, rows, columns> operator+(const Matrix2D<T, rows, columns>& right_hand_side) const {
        Matrix2D<T, rows, columns> new_matrix{}; 
    
        for (std::size_t new_row = 0; new_row < rows; new_row++) {
            for (std::size_t new_column = 0; new_column < columns; new_column++) {
                new_matrix(new_row, new_column) = matrix[new_row][new_column] + right_hand_side(new_row, new_column); 
            }
        }

        return new_matrix;
    }

    // A = B + scalar
    Matrix2D<T, rows, columns> operator+(const T& scalar) const {
        Matrix2D<T, rows, columns> new_matrix{}; 
    
        for (std::size_t new_row = 0; new_row < rows; new_row++) {
            for (std::size_t new_column = 0; new_column < columns; new_column++) {
                new_matrix(new_row, new_column) = matrix[new_row][new_column] + scalar; 
            }
        }

        return new_matrix;
    }

    // A = B - C
    Matrix2D<T, rows, columns> operator-(const Matrix2D<T, rows, columns>& right_hand_side) const {
        Matrix2D<T, rows, columns> new_matrix{}; 
    
        for (std::size_t new_row = 0; new_row < rows; new_row++) {
            for (std::size_t new_column = 0; new_column < columns; new_column++) {
                new_matrix(new_row, new_column) = matrix[new_row][new_column] - right_hand_side(new_row, new_column); 
            }
        }

        return new_matrix;
    }

    // A = B - scalar
    Matrix2D<T, rows, columns> operator-(const T& scalar) const {
        Matrix2D<T, rows, columns> new_matrix{}; 
    
        for (std::size_t new_row = 0; new_row < rows; new_row++) {
            for (std::size_t new_column = 0; new_column < columns; new_column++) {
                new_matrix(new_row, new_column) = matrix[new_row][new_column] - scalar; 
            }
        }

        return new_matrix;
    }

    // A = B / scalar
    Matrix2D<T, rows, columns> operator/(const T& scalar) const {
        Matrix2D<T, rows, columns> new_matrix{}; 
    
        for (std::size_t new_row = 0; new_row < rows; new_row++) {
            for (std::size_t new_column = 0; new_column < columns; new_column++) {
                new_matrix(new_row, new_column) = matrix[new_row][new_column] / scalar; 
            }
        }

        return new_matrix;
    }

    Matrix2D<T, rows, columns> elementWiseMultilply(const Matrix2D<T, rows, columns>& right_hand_side) const {
        Matrix2D<T, rows, columns> new_matrix{}; 
    
        for (std::size_t new_row = 0; new_row < rows; new_row++) {
            for (std::size_t new_column = 0; new_column < columns; new_column++) {
                new_matrix(new_row, new_column) = matrix[new_row][new_column] * right_hand_side(new_row, new_column); 
            }
        }

        return new_matrix;
    }

    Matrix2D<T, rows, columns> elementWiseDivision(const Matrix2D<T, rows, columns>& right_hand_side) const {
        Matrix2D<T, rows, columns> new_matrix{}; 
    
        for (std::size_t new_row = 0; new_row < rows; new_row++) {
            for (std::size_t new_column = 0; new_column < columns; new_column++) {
                new_matrix(new_row, new_column) = matrix[new_row][new_column] / right_hand_side(new_row, new_column); 
            }
        }

        return new_matrix;
    }

    void setZeros() {
        for (std::size_t row = 0; row < rows; row++) {
            for (std::size_t column = 0; column < columns; column++) {
                matrix[row][column] = T{0};
           }
        }
    }

    void setIdentity() {
        static_assert(rows == columns, "Identity requires square matrix.");
        setZeros();

        for (std::size_t diag_index = 0; diag_index < rows; diag_index++) {
            matrix[diag_index][diag_index] = T{1};
        }
    }

    // Generic transpose must return another matrix if the transposed matrix is 
    // not square because the outputted matrix would be of a different type (i.e. size)  
    Matrix2D<T, columns, rows> transpose() const {
        Matrix2D<T, columns, rows> output;

        for (std::size_t row = 0; row < rows; row++) {
            for (std::size_t column = 0; column < columns; column++) {
                output(column, row) = matrix[row][column];
            }
        }

        return output;
    }

    // In-place transpose can simply swap the current rows and columns and the output is 
    // the same type (i.e size)
    void inPlaceTranspose() {
        static_assert(rows == columns, "In-place transpose only valid for square matrices.");

        for (std::size_t i = 0; i < rows; ++i) {
            for (std::size_t j = i + 1; j < columns; ++j) {
                std::swap(matrix[i][j], matrix[j][i]);
            }
        }
    }

    Matrix2D<T, rows, columns> power(const int exponent) {
        // static_assert(rows == columns, "Raising a matrix to a power is only valid for square matrices.");

        // Matrix2D<T, rows, columns> result = identityMatrix<T, rows>;
        // TBD - Need inverse function for negative powers!
    }

    T det() const {
        static_assert(rows == columns, "Determinant only valid for square matrices.");
        static_assert(std::is_floating_point<T>::value,
                    "Gaussian elimination in determinant function requires double or float matrix type");

        Matrix2D<T, rows, columns> matrix_copy(*this);
        int determinate_sign = 1; 
        T determinate        = static_cast<T>(1);
        std::array<T, rows> row_scale; 

        // Step 1: Compute row scale factors
        for (std::size_t row = 0; row < rows; row++) {
            T row_max = static_cast<T>(0);
            for (std::size_t column = 0; column < columns; column++) {
                row_max = std::max(row_max, std::abs(matrix_copy(row, column)));
            }
            // prevent divide by zero later // fix this to be < e-12
            row_scale[row] = (row_max < static_cast<T>(1e-12)) ? static_cast<T>(1) : row_max;
        }

        for (std::size_t pivot_index = 0; pivot_index < rows; pivot_index++) {                       
            // Step 2: Find best pivot row (scaled partial pivoting)
            std::size_t best_row = pivot_index; 
            T largest_ratio      = std::abs(matrix_copy(pivot_index, pivot_index)) / row_scale[pivot_index];
            
            for (std::size_t current_row = pivot_index + 1; current_row < rows; current_row++) {
                T current_ratio = std::abs(matrix_copy(current_row, pivot_index)) / row_scale[current_row];
                if (current_ratio > largest_ratio) {
                    largest_ratio = current_ratio; 
                    best_row      = current_row; 
                }
            }

            // Step 3: Swap rows if needed
            if (best_row != pivot_index) {
                for (std::size_t i = 0; i < columns; i++) {
                    std::swap(matrix_copy(pivot_index, i), matrix_copy(best_row, i));
                }
                std::swap(row_scale[pivot_index], row_scale[best_row]);
                determinate_sign = -determinate_sign; 
            }

            // Step 4: Check pivot element AFTER swap
            T pivot_value = matrix_copy(pivot_index, pivot_index);
            if (std::abs(pivot_value) < static_cast<T>(1e-12)) {
                return static_cast<T>(0); 
            }

            // Step 5: Eliminate below pivot
            for (std::size_t row = pivot_index + 1; row < rows; row++) {
                T factor = matrix_copy(row, pivot_index) / pivot_value;
                for (std::size_t column = pivot_index; column < columns; column++) {
                    matrix_copy(row, column) -= factor * matrix_copy(pivot_index, column);
                }
            }  
        }

        // Step 6: Product of diagonal entries
        for (std::size_t i = 0; i < rows; i++) {
            determinate *= matrix_copy(i, i);
        }
        
        return determinate * static_cast<T>(determinate_sign);
    }   

private:


};


// Enables scalar * matrix (e.g. 5 * matrix), which not supported by member functions.
// Member functions can only handle matrix * scalar because they cannot accept two input arguments.
// Defined outside the class so the scalar can be the left-hand operand.
template<typename T, std::size_t rows, std::size_t columns>
Matrix2D<T, rows, columns> operator*(const T& scalar, const Matrix2D<T, rows, columns>& matrix) {
    return matrix * scalar;
}

template<typename T, std::size_t rows, std::size_t columns>
Matrix2D<T, rows, columns> operator+(const T& scalar, const Matrix2D<T, rows, columns>& matrix) {
    return matrix + scalar;
}

// Factory functions like zerosMatrix and identityMatrix are defined outside the class
// because they create and return new Matrix2D instances without needing an existing object.
// Defining them as free functions enables clean syntax and avoids coupling these utilities
// to any particular instance of the class.
template<typename T, std::size_t square_matrix_size>
Matrix2D<T, square_matrix_size, square_matrix_size> identityMatrix() {
    Matrix2D<T, square_matrix_size, square_matrix_size> identityMat;
    identityMat.setIdentity();
    return identityMat;
}

template<typename T, std::size_t num_rows, std::size_t num_columns>
Matrix2D<T, num_rows, num_columns> zerosMatrix() {
    Matrix2D<T, num_rows, num_columns> zerosMat;
    zerosMat.setZeros();
    return zerosMat;
}

#endif
