// Copyright 2025 Zachary Hart

#include <array>
#include <iostream>

#include <math_utilities/matrix/matrix.hpp>
#include <math_utilities/quaternion/quat.hpp>
#include <math_utilities/vector/vector.hpp>

template <typename T> T pi(3.1415926535);

int main() {
  // quat<int> q;
  // q.print();

  quat<double> q1 = {0.5, 0.5, 0.5, 0.5};
  q1.print();

  quat<double> q2 = {1, 2, 3, 4};
  q2.norm();
  q2.print();

  quat<double> q3 = q2 * q1;
  q3.print();

  // q1.setIdentity();
  // q1.print();

  // q1 = {1,2,3,4};
  // q1.print();

  // std::cout << q1.scalar() << "\n\n";

  // vector<double, 3> vector_part = q1.vec();
  // vector_part.print();

  vector<double, 3> vec = {1, 2, 3};
  vec.print();
  vec = q1 * vec;
  vec.print();

  return 0;
};
