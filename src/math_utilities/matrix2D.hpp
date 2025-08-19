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
    
    Matrix2D() {
        setZeros();
    }; 

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

    T& operator()(std::size_t desired_row, std::size_t desired_column) {
        return matrix[desired_row][desired_column];
    }

    const T& operator()(std::size_t desired_row, std::size_t desired_column) const {
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

    Matrix2D<T, rows, columns>& set_row(std::size_t row_index, std::initializer_list<T> row_values) {
        if (row_index >= rows)            {throw std::out_of_range("Row index out of range");}
        if (row_values.size() != columns) {throw std::invalid_argument("Initializer list size must match number of columns");}

        Matrix2D<T, rows, columns>& new_matrix = *this; 
        std::size_t new_column = 0;

        for (const T& value : row_values) {
            new_matrix(row_index, new_column++) = value;
        }

        return new_matrix; 
    }

    Matrix2D<T, rows, columns>& set_column(std::size_t column_index, std::initializer_list<T> column_values) {
        if (column_index >= columns)      {throw std::out_of_range("Column index out of range");}
        if (column_values.size() != rows) {throw std::invalid_argument("Initializer list size must match number of rows");}

        Matrix2D<T, rows, columns>& new_matrix = *this; 
        std::size_t new_row = 0;

        for (const T& value : column_values) {
            new_matrix(new_row++, column_index) = value;
        }

        return new_matrix; 
    }

    // A = B * C
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

    // A = B * scalar
    Matrix2D<T, rows, columns> operator*(const T& scalar) const {
       
        const Matrix2D<T, rows, columns>& right_hand_side = *this; 
        Matrix2D<T, rows, columns> new_matrix{}; 
        
        for (std::size_t new_row = 0; new_row < rows; new_row++) {
            for (std::size_t new_column = 0; new_column < columns; new_column++) {
                    new_matrix(new_row, new_column) = scalar * right_hand_side(new_row, new_column);
            }
        }
        
        return new_matrix; 
    }

    // A = B + C
    Matrix2D<T, rows, columns> operator+(const Matrix2D<T, rows, columns>& right_hand_side) const {

        const Matrix2D<T, rows, columns>& left_hand_side = *this; 
        Matrix2D<T, rows, columns> new_matrix{}; 
    
        for (std::size_t new_row = 0; new_row < rows; new_row++) {
            for (std::size_t new_column = 0; new_column < columns; new_column++) {
                new_matrix(new_row, new_column) = left_hand_side(new_row, new_column) + right_hand_side(new_row, new_column); 
            }
        }

        return new_matrix;
    }

    // A = B + scalar
    Matrix2D<T, rows, columns> operator+(const T& scalar) const {

        const Matrix2D<T, rows, columns>& left_hand_side = *this; 
        Matrix2D<T, rows, columns> new_matrix{}; 
    
        for (std::size_t new_row = 0; new_row < rows; new_row++) {
            for (std::size_t new_column = 0; new_column < columns; new_column++) {
                new_matrix(new_row, new_column) = left_hand_side(new_row, new_column) + scalar; 
            }
        }

        return new_matrix;
    }

    // A = B - C
    Matrix2D<T, rows, columns> operator-(const Matrix2D<T, rows, columns>& right_hand_side) const {

        const Matrix2D<T, rows, columns>& left_hand_side = *this; 
        Matrix2D<T, rows, columns> new_matrix{}; 
    
        for (std::size_t new_row = 0; new_row < rows; new_row++) {
            for (std::size_t new_column = 0; new_column < columns; new_column++) {
                new_matrix(new_row, new_column) = left_hand_side(new_row, new_column) - right_hand_side(new_row, new_column); 
            }
        }

        return new_matrix;
    }

    // A = B - scalar
    Matrix2D<T, rows, columns> operator-(const T& scalar) const {

        const Matrix2D<T, rows, columns>& left_hand_side = *this; 
        Matrix2D<T, rows, columns> new_matrix{}; 
    
        for (std::size_t new_row = 0; new_row < rows; new_row++) {
            for (std::size_t new_column = 0; new_column < columns; new_column++) {
                new_matrix(new_row, new_column) = left_hand_side(new_row, new_column) - scalar; 
            }
        }

        return new_matrix;
    }

    // A = B / scalar
    Matrix2D<T, rows, columns> operator/(const T& scalar) const {

        const Matrix2D<T, rows, columns>& left_hand_side = *this; 
        Matrix2D<T, rows, columns> new_matrix{}; 
    
        for (std::size_t new_row = 0; new_row < rows; new_row++) {
            for (std::size_t new_column = 0; new_column < columns; new_column++) {
                new_matrix(new_row, new_column) = left_hand_side(new_row, new_column) / scalar; 
            }
        }

        return new_matrix;
    }

    Matrix2D<T, rows, columns> elementWiseMultilply(const Matrix2D<T, rows, columns>& right_hand_side) const {

        const Matrix2D<T, rows, columns>& left_hand_side = *this; 
        Matrix2D<T, rows, columns> new_matrix{}; 
    
        for (std::size_t new_row = 0; new_row < rows; new_row++) {
            for (std::size_t new_column = 0; new_column < columns; new_column++) {
                new_matrix(new_row, new_column) = left_hand_side(new_row, new_column) * right_hand_side(new_row, new_column); 
            }
        }

        return new_matrix;
    }

    Matrix2D<T, rows, columns> elementWiseDivision(const Matrix2D<T, rows, columns>& right_hand_side) const {

        const Matrix2D<T, rows, columns>& left_hand_side = *this; 
        Matrix2D<T, rows, columns> new_matrix{}; 
    
        for (std::size_t new_row = 0; new_row < rows; new_row++) {
            for (std::size_t new_column = 0; new_column < columns; new_column++) {
                new_matrix(new_row, new_column) = left_hand_side(new_row, new_column) / right_hand_side(new_row, new_column); 
            }
        }

        return new_matrix;
    }

    void setZeros() {
        Matrix2D<T, rows, columns>& current_matrix = *this;
        
        for (std::size_t row = 0; row < rows; row++) {
            for (std::size_t column = 0; column < columns; column++) {
                current_matrix(row, column) = T{0};
           }
        }
    }

    void setIdentity() {
        static_assert(rows == columns, "Identity requires square matrix.");
        Matrix2D<T, rows, columns>& current_matrix = *this;
        current_matrix.setZeros();

        for (std::size_t new_row = 0; new_row < rows; new_row++) {
                current_matrix(new_row, new_row) = T{1};
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
