// Copyright 2025 Zachary Hart

#include <array>
#include <iostream>

#include <math_utilities/matrix2D.hpp>
#include <math_utilities/vector.hpp>

template <typename T> T pi(3.1415926535);

int main() {
  // Matrix2D<double, 3,3> A = {2,1,1, 1,3,2, 1,0,0};
  // // A.print();

  // Matrix2D<double, 3,3> A_inv = A.inv();
  // A_inv.print();

  // Matrix2D<double, 3,3> A_inv_expected{0, 0, 1, -2, 1, 3,  3, -1, -5};
  // A_inv_expected.print();

  Matrix2D<double, 3, 3> A1 = {4, 7, 2, 3, 6, 1, 2, 5, 1};
  A1.print();

  Matrix2D<double, 3, 3> A1_inv = A1.inv();
  A1_inv.print();

  Matrix2D<double, 3, 3> A1_inv_expected = {
      1.0 / 3.0, 1.0, -5.0 / 3.0, -1.0 / 3.0, 0.0, 2.0 / 3.0, 1.0, -2.0, 1.0};
  A1_inv_expected.print();

  Matrix2D<double, 3, 3> A2 = {1, 2, 3, 0, 1, 4, 5, 6, 0};
  A2.print();

  Matrix2D<double, 3, 3> A2_inv = A2.inv();
  A2_inv.print();

  Matrix2D<double, 3, 3> A2_inv_expected = {-24, 18, 5, 20, -15, -4, -5, 4, 1};
  A2_inv_expected.print();

  Matrix2D<double, 3, 3> A3 = {2, -1, 0, -1, 2, -1, 0, -1, 2};
  A3.print();

  Matrix2D<double, 3, 3> A3_inv = A3.inv();
  A3_inv.print();

  Matrix2D<double, 3, 3> A3_inv_expected = {0.75, 0.5,  0.25, 0.5, 1.0,
                                            0.5,  0.25, 0.5,  0.75};
  A3_inv_expected.print();

  Matrix2D<double, 3, 3> A4 = {3, 0, 2, 2, 0, -2, 0, 1, 1};
  A4.print();

  Matrix2D<double, 3, 3> A4_inv = A4.inv();
  A4_inv.print();

  Matrix2D<double, 3, 3> A4_inv_expected = {0.2, 0.2, 0,    -0.2, 0.3,
                                            1,   0.2, -0.3, 0};
  A4_inv_expected.print();

  return 0;
}
