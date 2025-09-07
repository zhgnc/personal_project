// Copyright 2025 Zachary Hart

#include <array>
#include <iostream>

#include <math_utilities/matrix/matrix.hpp>
#include <math_utilities/quaternion/quat.hpp>
#include <math_utilities/vector/vector.hpp>

template <typename T> T pi(3.1415926535);

int main() {
  quat<int> q;
  q.print();

  quat<double> q1 = {0.5, 0.5, 0.5, 0.5};
  q1.print();

  q1 = {1, 2, 3, 4};
  q1.print();

  q1.setIdentity();
  q1.print();

  std::cout << q1(3) << "\n";

  return 0;
};
