// Copyright 2025 Zachary Hart

#include <array>
#include <iostream>

#include <math_utilities/matrix2D.hpp>
#include <math_utilities/vector.hpp>

template <typename T> T pi(3.1415926535);

int main() {
  Matrix2D<int, 3, 3> test_matrix_1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
  Matrix2D<int, 3, 3> test_matrix_2 = {10, 11, 12, 13, 14, 15, 16, 17, 18};
  
  Matrix2D<int, 3, 3> test_matrix_3 = test_matrix_1 * test_matrix_2;
  test_matrix_3.print();

  Matrix2D<int, 3, 2> test_matrix_4 = {1, 2, 3, 4, 5, 6};  
  Matrix2D<int, 2, 3> test_matrix_5 = {10, 11, 12, 13, 14, 15};
  
  Matrix2D<int, 3, 3> test_matrix_6 = test_matrix_4 * test_matrix_5;
  test_matrix_6.print();

  Matrix2D<int, 2, 2> test_matrix_7 = test_matrix_5 * test_matrix_4;
  test_matrix_7.print();

  Matrix2D<double, 3, 2> test_matrix_8 = {1, 2, 3, 4, 5, 6};  
  Matrix2D<double, 2, 3> test_matrix_9 = {10, 11, 12, 13, 14, 15};
  
  Matrix2D<double, 3, 3> test_matrix_10 = test_matrix_8 * test_matrix_9;
  test_matrix_10.print();

  std::cout << test_matrix_9.size << "\n";
  std::cout << test_matrix_9.num_rows << "\n";
  std::cout << test_matrix_9.num_columns << "\n\n";


  Matrix2D<int, 3,3> test_matrix_add = test_matrix_1 + test_matrix_2;
  test_matrix_add.print(); 
  Matrix2D<int, 3,3> test_matrix_sub = test_matrix_1 - test_matrix_2; 
  test_matrix_sub.print();
  Matrix2D<int, 3,3> test_matrix_element_mult = test_matrix_1.elementWiseMultilply(test_matrix_1); 
  test_matrix_element_mult.print();

  Matrix2D<int, 3,3> test_matrix_element_div = test_matrix_1.elementWiseDivision(test_matrix_1); 
  test_matrix_element_div.print();

  Matrix2D<int, 3,3> test_matrix_times_const_1 = test_matrix_1 * 10; 
  test_matrix_times_const_1.print();

  Matrix2D<int, 3,3> test_matrix_times_const_2 = 10 * test_matrix_1; 
  test_matrix_times_const_2.print();

  Matrix2D<int, 3,3> test_mat_write_to_element = test_matrix_1;
  test_mat_write_to_element(1,2) = 100;
  test_mat_write_to_element.print();

  test_matrix_1.set_row(1, {100, 101, 102});
  test_matrix_1.print();

  test_matrix_1.set_column(2, {1000, 1001, 1002});
  test_matrix_1.print();

  return 0;
}
