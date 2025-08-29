#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <array>
#include <limits>
#include <cmath>

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

    Matrix2D<T, rows, columns>& operator*=(const Matrix2D<T, rows, columns>& rhs) {
        static_assert(rows == columns, "In-place multiplication only valid for square matrices.");
        
        *this = (*this) * rhs;
        return *this;
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
        static_assert(std::is_floating_point<T>::value, "Gaussian elimination in determinant function requires double/float matrix type");

        Matrix2D<T, rows, columns> matrix_copy(*this);

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
    
    Matrix2D<T, rows, columns> inv() const {
        static_assert(rows == columns, "Determinant only valid for square matrices.");
        static_assert(std::is_floating_point<T>::value, "Gaussian elimination in determinant function requires double/float matrix type");

        Matrix2D<T, rows, columns> P;
        Matrix2D<T, rows, columns> L;
        Matrix2D<T, rows, columns> U(*this);
        Matrix2D<T, rows, columns> inverse_matrix;

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
            Matrix2D<T, rows, 1> y;

            for (std::size_t row = 0; row < rows; row++) {
                y(row, 0) = P(row,column);

                for (std::size_t j = 0; j < row; j++) {
                    y(row, 0) -= L(row, j) * y(j, 0);
                }
            }

            // Backwards substitution solve Ux = y
            Matrix2D<T, rows, 1> x;

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
    Matrix2D<T, rows, columns> pow(int exponent) const {
        static_assert(rows == columns, "Matrix power only defined for square matrices.");

        Matrix2D<T, rows, columns> new_matrix;
        Matrix2D<T, rows, columns> matrix_copy(*this);
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
