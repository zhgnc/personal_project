#ifndef MATRIX_DECLARATIONS_HPP
#define MATRIX_DECLARATIONS_HPP

#include <iostream>
#include <array>
#include <limits>
#include <cmath>
#include <cassert>
#include <type_traits>
#include <initializer_list>

template <typename type, std::size_t rows, std::size_t columns>
class matrix
{
public:
    static_assert(std::is_arithmetic_v<type>, "matrix requires numeric types.");
    std::array<std::array<type, columns>, rows> data;

    static constexpr std::size_t num_rows    = rows;
    static constexpr std::size_t num_columns = columns;
    static constexpr std::size_t size        = rows * columns;

    // Constructors
    matrix();
    matrix(std::initializer_list<type> initial_matrix);
    

    // In matrix_creators.hpp
    // matrix<type, rows, columns> diag_matrix(const matrix<type, rows, 1>& diag_values);
    // matrix<type, N, N> identity_matrix();
    // matrix<type, N, N> skew_matrix(const matrix<type, (N * (N - 1)) / 2, 1>& skew_vec);


    // Utilities
    void print() const;
    void set_zeros();
    void set_identity();
    void set_row(std::size_t row_index, std::initializer_list<type> row_values);
    void set_column(std::size_t column_index, std::initializer_list<type> column_values);
    type trace() const;
    matrix<type, rows, 1> get_diag() const;
    void set_diag(const matrix<type, rows, 1>& diag_values);
    type& operator()(std::size_t desired_row, std::size_t desired_column);
    const type& operator()(std::size_t desired_row, std::size_t desired_column) const;

    template <std::size_t block_rows, std::size_t block_columns> 
    void set_block(std::size_t starting_row, std::size_t starting_column, const matrix<type, block_rows, block_columns>& block_to_insert);
    
    template <std::size_t block_rows, std::size_t block_columns> 
    matrix<type, block_rows, block_columns> get_block(std::size_t starting_row, std::size_t starting_column) const;

    template <std::size_t skew_vec_length>
    void set_skew(const matrix<type, skew_vec_length, 1>& skew_vector);

    template <typename U>
    explicit operator matrix<U, rows, columns>() const; 


    // Basic Math
    template <std::size_t other_columns>
    matrix<type, rows, other_columns> operator*(const matrix<type, columns, other_columns>& right_hand_side) const;
    matrix<type, rows, columns> operator*(const type& scalar) const;
    matrix<type, rows, columns>& operator*=(const matrix<type, rows, columns>& right_hand_side);
    matrix<type, rows, columns> operator+(const matrix<type, rows, columns>& right_hand_side) const;
    matrix<type, rows, columns> operator+(const type& scalar) const;
    matrix<type, rows, columns> operator-(const matrix<type, rows, columns>& right_hand_side) const;
    matrix<type, rows, columns> operator-(const type& scalar) const;
    matrix<type, rows, columns> operator-() const;
    matrix<type, rows, columns> operator/(const type& scalar) const;


    // Advanced Math
    matrix<type, rows, columns> element_wise_multiply(const matrix<type, rows, columns>& right_hand_side) const;
    matrix<type, rows, columns> element_wise_division(const matrix<type, rows, columns>& right_hand_side) const;
    matrix<type, columns, rows> T() const;
    void in_place_transpose();
    type det() const;
    matrix<type, rows, columns> inv() const;
    matrix<type, rows, columns> pow(int exponent) const;
};

#endif