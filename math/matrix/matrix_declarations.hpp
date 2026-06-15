#ifndef MATRIX_DECLARATIONS_HPP
#define MATRIX_DECLARATIONS_HPP

#include <iostream>
#include <array>
#include <limits>
#include <cmath>
#include <cassert>
#include <type_traits>
#include <initializer_list>

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
    

    // In matrix_creators.hpp
    // matrix<T, rows, columns> diag_matrix(const matrix<T, rows, 1>& diag_values);
    // matrix<T, N, N> identity_matrix();
    // matrix<T, N, N> skew_matrix(const matrix<T, (N * (N - 1)) / 2, 1>& skew_vec);


    // Utilities
    void print() const;
    void set_zeros();
    void set_identity();
    void set_row(std::size_t row_index, std::initializer_list<T> row_values);
    void set_column(std::size_t column_index, std::initializer_list<T> column_values);
    T trace() const;
    matrix<T, rows, 1> get_diag() const;
    void set_diag(const matrix<T, rows, 1>& diag_values);
    T& operator()(std::size_t desired_row, std::size_t desired_column);
    const T& operator()(std::size_t desired_row, std::size_t desired_column) const;

    template <std::size_t block_rows, std::size_t block_columns> 
    void set_block(std::size_t starting_row, std::size_t starting_column, const matrix<T, block_rows, block_columns>& block_to_insert);
    
    template <std::size_t block_rows, std::size_t block_columns> 
    matrix<T, block_rows, block_columns> get_block(std::size_t starting_row, std::size_t starting_column) const;

    template <std::size_t skew_vec_length>
    void set_skew(const matrix<T, skew_vec_length, 1>& skew_vector);

    
    template <typename U>
    explicit operator matrix<U, rows, columns>() const; 


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
    matrix<T, rows, columns> element_wise_multiply(const matrix<T, rows, columns>& right_hand_side) const;
    matrix<T, rows, columns> element_wise_division(const matrix<T, rows, columns>& right_hand_side) const;
    matrix<T, columns, rows>transpose() const;
    void in_place_transpose();
    T det() const;
    matrix<T, rows, columns> inv() const;
    matrix<T, rows, columns> pow(int exponent) const;
};

#endif