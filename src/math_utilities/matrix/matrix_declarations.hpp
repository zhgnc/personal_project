#ifndef MATRIX_DECLARATIONS_HPP
#define MATRIX_DECLARATIONS_HPP

#include <iostream>
#include <array>
#include <limits>
#include <cmath>

template <typename T, std::size_t rows, std::size_t columns>
class matrix 
{
public:
    static_assert(std::is_arithmetic_v<T>, "matrix requires numeric types.");
    std::array<std::array<T, columns>, rows> data;
    static constexpr std::size_t num_rows    = rows;
    static constexpr std::size_t num_columns = columns;
    static constexpr std::size_t size        = rows * columns;

    // Constructors
    matrix();
    matrix(std::initializer_list<T> initial_matrix);

    // Utilities
    void print() const;
    void setZeros();
    void setIdentity();
    void set_row(std::size_t row_index, std::initializer_list<T> row_values);
    void set_column(std::size_t column_index, std::initializer_list<T> column_values);
    T trace() const;

    
    // Operators
    T& operator()(std::size_t desired_row, std::size_t desired_column);
    const T& operator()(std::size_t desired_row, std::size_t desired_column) const;


    // Basic Math
    template <std::size_t other_columns>
    matrix<T, rows, other_columns> operator*(const matrix<T, columns, other_columns>& right_hand_side) const;
    matrix<T, rows, columns> operator*(const T& scalar) const;
    matrix<T, rows, columns>& operator*=(const matrix<T, rows, columns>& right_hand_side);
    matrix<T, rows, columns> operator+(const matrix<T, rows, columns>& right_hand_side) const;
    matrix<T, rows, columns> operator+(const T& scalar) const;
    matrix<T, rows, columns> operator-(const matrix<T, rows, columns>& right_hand_side) const;
    matrix<T, rows, columns> operator-(const T& scalar) const;
    matrix<T, rows, columns> operator-() const;
    matrix<T, rows, columns> operator/(const T& scalar) const;


    // Advanced Math
    matrix<T, rows, columns> elementWiseMultiply(const matrix<T, rows, columns>& right_hand_side) const;
    matrix<T, rows, columns> elementWiseDivision(const matrix<T, rows, columns>& right_hand_side) const;
    matrix<T, columns, rows> transpose() const;
    void inPlaceTranspose();
    T det() const;
    matrix<T, rows, columns> inv() const;
    matrix<T, rows, columns> pow(int exponent) const;
};

#endif