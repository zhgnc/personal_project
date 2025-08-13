// Copyright 2025 Zachary Hart

#include <array>
#include <iostream>

#include <math_utilities/vector.hpp>
#include <math_utilities/matrix2D.hpp>

template <typename T> T pi(3.1415926535);

int main() {
  Vector<int, 3> test_vec = {1, 2, 3};

  for (std::size_t i = 0; i < 3; i++) {
    std::cout << test_vec[i] << '\n';
  }

  test_vec[1] = 42;

  std::cout << "After modification:\n";
  for (std::size_t i = 0; i < 3; i++) {
    std::cout << test_vec[i] << '\n';
  }

  Vector<int, 3> test_vec_2 = {1, 2, 3};

  Vector<int, 3> output;
  output = test_vec * test_vec_2;

  std::cout << "After multiplication:\n";
  for (std::size_t i = 0; i < 3; i++) {
    std::cout << output[i] << '\n'; 
  }

  // for (int i = 0; i < 1000000; i++) {
  //   if (i % 1000 == 0) {
  //       std::cout << i << '\n';
  //   }
  // }

  Matrix2D<int, 2, 3> matrix = {1, 2, 3, 4, 5, 6};
  matrix.print();

  Matrix2D<int, 3, 2> matrix_2 = {1, 2, 3, 4, 5, 6};
  matrix_2.print();

  std::cout << matrix(1,5) << "\n";

  Matrix2D<int, 2, 2> new_matrix = matrix * matrix_2;
  new_matrix.print(); 

  // Matrix2D<int, 2, 2> new_matrix_2 = matrix_2 * matrix;
  // new_matrix.print(); 


  return 0;
}
