// Copyright 2025 Zachary Hart

#include <array>
#include <iostream>

#include <math_utilities/matrix2D.hpp>
#include <math_utilities/vector.hpp>

template <typename T> T pi(3.1415926535);

template <typename T> long long det3x3_exact(const Matrix2D<T, 3, 3> &A) {
  auto a00 = static_cast<long long>(A(0, 0));
  auto a01 = static_cast<long long>(A(0, 1));
  auto a02 = static_cast<long long>(A(0, 2));
  auto a10 = static_cast<long long>(A(1, 0));
  auto a11 = static_cast<long long>(A(1, 1));
  auto a12 = static_cast<long long>(A(1, 2));
  auto a20 = static_cast<long long>(A(2, 0));
  auto a21 = static_cast<long long>(A(2, 1));
  auto a22 = static_cast<long long>(A(2, 2));
  return a00 * (a11 * a22 - a12 * a21) - a01 * (a10 * a22 - a12 * a20) +
         a02 * (a10 * a21 - a11 * a20);
}

template <typename T> long double det4x4_exact(const Matrix2D<T, 4, 4> &A) {
  auto ld = [](T x) { return static_cast<long double>(x); };

  // Inline 3x3 determinant helper (Sarrus)
  auto det3 = [](long double b00, long double b01, long double b02,
                 long double b10, long double b11, long double b12,
                 long double b20, long double b21,
                 long double b22) -> long double {
    return b00 * (b11 * b22 - b12 * b21) - b01 * (b10 * b22 - b12 * b20) +
           b02 * (b10 * b21 - b11 * b20);
  };

  // Cofactor minors (remove row 0, remove col j)
  long double M11 =
      det3(ld(A(1, 1)), ld(A(1, 2)), ld(A(1, 3)), ld(A(2, 1)), ld(A(2, 2)),
           ld(A(2, 3)), ld(A(3, 1)), ld(A(3, 2)), ld(A(3, 3)));

  long double M12 =
      det3(ld(A(1, 0)), ld(A(1, 2)), ld(A(1, 3)), ld(A(2, 0)), ld(A(2, 2)),
           ld(A(2, 3)), ld(A(3, 0)), ld(A(3, 2)), ld(A(3, 3)));

  long double M13 =
      det3(ld(A(1, 0)), ld(A(1, 1)), ld(A(1, 3)), ld(A(2, 0)), ld(A(2, 1)),
           ld(A(2, 3)), ld(A(3, 0)), ld(A(3, 1)), ld(A(3, 3)));

  long double M14 =
      det3(ld(A(1, 0)), ld(A(1, 1)), ld(A(1, 2)), ld(A(2, 0)), ld(A(2, 1)),
           ld(A(2, 2)), ld(A(3, 0)), ld(A(3, 1)), ld(A(3, 2)));

  // Expansion along first row: + - + -
  return ld(A(0, 0)) * M11 - ld(A(0, 1)) * M12 + ld(A(0, 2)) * M13 -
         ld(A(0, 3)) * M14;
}

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

  Matrix2D<int, 3, 3> test_matrix_add = test_matrix_1 + test_matrix_2;
  test_matrix_add.print();
  Matrix2D<int, 3, 3> test_matrix_sub = test_matrix_1 - test_matrix_2;
  test_matrix_sub.print();
  Matrix2D<int, 3, 3> test_matrix_element_mult =
      test_matrix_1.elementWiseMultilply(test_matrix_1);
  test_matrix_element_mult.print();

  Matrix2D<int, 3, 3> test_matrix_element_div =
      test_matrix_1.elementWiseDivision(test_matrix_1);
  test_matrix_element_div.print();

  Matrix2D<int, 3, 3> test_matrix_times_const_1 = test_matrix_1 * 10;
  test_matrix_times_const_1.print();

  Matrix2D<int, 3, 3> test_matrix_times_const_2 = 10 * test_matrix_1;
  test_matrix_times_const_2.print();

  Matrix2D<int, 3, 3> test_mat_write_to_element = test_matrix_1;
  test_mat_write_to_element(1, 2) = 100;
  test_mat_write_to_element.print();

  test_matrix_1.set_row(1, {100, 101, 102});
  test_matrix_1.print();

  test_matrix_1.set_column(2, {1000, 1001, 1002});
  test_matrix_1.print();

  Matrix2D<float, 3, 3> init_zero_matrix;
  init_zero_matrix.print();
  init_zero_matrix.setIdentity();
  init_zero_matrix.print();
  test_matrix_1.print();
  test_matrix_1.setZeros();
  test_matrix_1.print();
  test_matrix_2.print();
  test_matrix_2.setIdentity();
  test_matrix_2.print();

  Matrix2D<double, 3, 3> mat_init_eye = identityMatrix<double, 3>();
  mat_init_eye.print();

  Matrix2D<double, 3, 2> mat_init_zeros = zerosMatrix<double, 3, 2>();
  mat_init_zeros.print();

  Matrix2D<int, 2, 2> i2 = identityMatrix<int, 2>();
  Matrix2D<int, 2, 2> test_addition_matrix_1 = i2 + 5;
  test_addition_matrix_1.print();
  Matrix2D<int, 2, 2> test_addition_matrix_2 = 5 + i2;
  test_addition_matrix_2.print();

  Matrix2D<int, 2, 2> test_sub_matrix_1 = i2 - 5;
  test_sub_matrix_1.print();

  Matrix2D<float, 3, 3> i3 = identityMatrix<float, 3>();
  Matrix2D<float, 3, 3> test_divide_by_scalar = i3 / 10;
  test_divide_by_scalar.print();

  test_matrix_1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  test_matrix_1.print();
  test_matrix_1.inPlaceTranspose();
  test_matrix_1.print();

  Matrix2D<int, 3, 2> test_matrix_14 = {1, 2, 3, 4, 5, 6};
  test_matrix_14.print();
  Matrix2D<int, 2, 3> test_matrix_14_T;
  test_matrix_14_T = test_matrix_14.transpose();
  test_matrix_14_T.print();

  Matrix2D<double, 3, 3> I = {1, 0, 0, 0, 1, 0, 0, 0, 1};
  std::cout << "det(I) = " << I.det() << " (expected 1)\n";

  Matrix2D<double, 3, 3> D = {2, 0, 0, 0, 3, 0, 0, 0, 4};
  std::cout << "det(D) = " << D.det() << " (expected 24)\n";

  Matrix2D<double, 3, 3> U = {1, 2, 3, 0, 4, 5, 0, 0, 6};
  std::cout << "det(U) = " << U.det() << " (expected 24)\n";

  Matrix2D<double, 2, 2> S = {2, 4, 1, 2};
  std::cout << "det(S) = " << S.det() << " (expected 0)\n";

  Matrix2D<double, 2, 2> N = {0, 1, 1, 0};
  std::cout << "det(N) = " << N.det() << " (expected -1)\n";

  Matrix2D<double, 3, 3> M = {2, 1, 5, 0, 3, -1, 0, 0, 4};
  std::cout << "det(M) = " << M.det() << " (expected 24)\n";

  Matrix2D<double, 3, 3> M1 = {7, -3, 2, 4, 5, -6, 1, 8, 9};
  std::cout << "det(M1) = " << M1.det() << " (expected 831)\n";

  Matrix2D<double, 3, 3> M2 = {-12, 7, 5, 3, -9, 14, 6, 6, 8};
  std::cout << "det(M2) = " << M2.det() << " (expected 2652)\n";

  Matrix2D<double, 4, 4> M3 = {2, 5,  3, 7, -1, 4, 0, 2,
                               3, -2, 6, 4, 1,  0, 5, -3};
  std::cout << "det(M3) = " << M3.det() << " (expected -292)\n";

  Matrix2D<double, 4, 4> M4 = {10, -7, 3, 4, 2, 0,  -5, 6,
                               -3, 8,  9, 1, 5, -2, 7,  -6};
  std::cout << "det(M4) = " << M4.det() << " (expected -3191)\n";

  Matrix2D<double, 4, 4> M5 = {6,  1, -2, 8, 3, 5,  7, -4,
                               -1, 9, 2,  6, 4, -3, 8, 0};
  std::cout << "det(M5) = " << M5.det() << " (expected -5432)\n";

  std::cout << "det(M1 exact) = " << det3x3_exact(M1)
            << ", det(GE) = " << M1.det() << " (expected 831)\n";

  std::cout << "det(M2 exact) = " << det3x3_exact(M2)
            << ", det(GE) = " << M2.det() << " (expected 2652)\n";
  std::cout << "det(M3 exact) = " << det4x4_exact(M3)
            << ", det(GE) = " << M3.det() << " (expected -292)\n";

  std::cout << "det(M4 exact) = " << det4x4_exact(M4)
            << ", det(GE) = " << M4.det() << " (expected -3191)\n";

  std::cout << "det(M5 exact) = " << det4x4_exact(M5)
            << ", det(GE) = " << M5.det() << " (expected -5432)\n";

  // Harder determinant tests (precision stress cases)
  Matrix2D<double, 3, 3> H1 = {1e10, 1e10, 1e10, 1e10,       1e10 + 1e-5,
                               1e10, 1e10, 1e10, 1e10 + 1e-5};
  // Very large, almost singular. True det ~ 100000.0
  std::cout << "det(H1) = " << H1.det() << " (expected ~1e5)\n";

  Matrix2D<double, 4, 4> H2 = {
      1.0, 1.0, 1.0,         1.0, 1.0, 1.0 + 1e-12, 1.0, 1.0,
      1.0, 1.0, 1.0 + 1e-12, 1.0, 1.0, 1.0,         1.0, 1.0 + 1e-12};
  // Nearly singular (all rows almost identical). True det ~ 1e-36
  std::cout << "det(H2) = " << H2.det() << " (expected ~1e-36)\n";

  Matrix2D<double, 3, 3> H3 = {1, 1, 1, 1, 1 + 1e-12, 1, 1, 1, 1 + 1e-12};
  // Another cancellation-heavy case. Expected det ~ 1e-24
  std::cout << "det(H3) = " << H3.det() << " (expected ~1e-24)\n";

  // Classic Hilbert matrix (n=4). Known to be extremely ill-conditioned
  Matrix2D<double, 4, 4> H4 = {1.0,       1.0 / 2.0, 1.0 / 3.0, 1.0 / 4.0,
                               1.0 / 2.0, 1.0 / 3.0, 1.0 / 4.0, 1.0 / 5.0,
                               1.0 / 3.0, 1.0 / 4.0, 1.0 / 5.0, 1.0 / 6.0,
                               1.0 / 4.0, 1.0 / 5.0, 1.0 / 6.0, 1.0 / 7.0};
  // Exact det = 1 / 604800 ≈ 1.653439e-6
  std::cout << "det(H4) = " << H4.det() << " (expected ~1.653439e-6)\n";

  return 0;
}
