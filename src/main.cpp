// Copyright 2025 Zachary Hart

#include <iostream>
#include <array>

#include <math_utilities/vector.hpp>

template <typename T> T pi(3.1415926535);

int main() {
  std::cout << pi<int> << std::endl;
  std::cout << pi<float> << std::endl;
  std::cout << pi<double> << std::endl;

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

  return 0;
}
