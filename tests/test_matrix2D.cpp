#include <gtest/gtest.h>
#include "../src/math_utilities/matrix2D.hpp"
#include <Eigen/Dense>
#include <random>
#include <limits> 

// Test constructor and element access
TEST(Matrix2DTest, ElementAccess) {
    Matrix2D<int, 2, 2> mat = {1, 2, 3, 4};
    EXPECT_EQ(mat(0, 0), 1);
    EXPECT_EQ(mat(0, 1), 2);
    EXPECT_EQ(mat(1, 0), 3);
    EXPECT_EQ(mat(1, 1), 4);

    mat(1,0) = 100; 
    EXPECT_EQ(mat(1,0), 100);
}

TEST(Matrix2DTest, MatrixMultiplicationInt) {   
    Matrix2D<int, 3, 3> test_matrix_1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
    Matrix2D<int, 3, 3> test_matrix_2 = {10, 11, 12, 13, 14, 15, 16, 17, 18};
    Matrix2D<int, 3, 3> output_1      = test_matrix_1 * test_matrix_2;
    Matrix2D<int, 3, 3> expected_1    = {84, 90, 96, 201, 216, 231, 318, 342, 366};

    for (size_t rows = 0; rows < expected_1.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_1.num_columns; columns++) {
            EXPECT_EQ(expected_1(rows, columns), output_1(rows, columns));
        }
    }

    Matrix2D<int, 3, 2> test_matrix_3 = {1, 2, 3, 4, 5, 6};  
    Matrix2D<int, 2, 3> test_matrix_4 = {10, 11, 12, 13, 14, 15};
    Matrix2D<int, 3, 3> output_2      = test_matrix_3 * test_matrix_4;
    Matrix2D<int, 3, 3> expected_2    = {36, 39, 42, 82, 89, 96, 128, 139, 150};

    for (size_t rows = 0; rows < expected_2.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_2.num_columns; columns++) {
            EXPECT_EQ(expected_2(rows, columns), output_2(rows, columns));
        }
    }

    Matrix2D<int, 2, 2> output_3   = test_matrix_4 * test_matrix_3;
    Matrix2D<int, 2, 2> expected_3 = {103, 136, 130, 172};

    for (size_t rows = 0; rows < expected_3.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_3.num_columns; columns++) {
            EXPECT_EQ(expected_3(rows, columns), output_3(rows, columns));
        }
    }

    Matrix2D<int, 3, 2> output_4   = test_matrix_1 * test_matrix_3;
    Matrix2D<int, 3, 2> expected_4 = {22, 28, 49, 64, 76, 100};

    for (size_t rows = 0; rows < expected_4.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_4.num_columns; columns++) {
            EXPECT_EQ(expected_4(rows, columns), output_4(rows, columns));
        }
    }
}

TEST(Matrix2DTest, MatrixMultiplicationFloat) {   
    Matrix2D<float, 3, 3> test_matrix_1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
    Matrix2D<float, 3, 3> test_matrix_2 = {10, 11, 12, 13, 14, 15, 16, 17, 18};
    Matrix2D<float, 3, 3> output_1      = test_matrix_1 * test_matrix_2;
    Matrix2D<float, 3, 3> expected_1    = {84, 90, 96, 201, 216, 231, 318, 342, 366};

    for (size_t rows = 0; rows < expected_1.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_1.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected_1(rows, columns), output_1(rows, columns));
        }
    }

    Matrix2D<float, 3, 2> test_matrix_3 = {1, 2, 3, 4, 5, 6};  
    Matrix2D<float, 2, 3> test_matrix_4 = {10, 11, 12, 13, 14, 15};
    Matrix2D<float, 3, 3> output_2      = test_matrix_3 * test_matrix_4;
    Matrix2D<float, 3, 3> expected_2    = {36, 39, 42, 82, 89, 96, 128, 139, 150};

    for (size_t rows = 0; rows < expected_2.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_2.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected_2(rows, columns), output_2(rows, columns));
        }
    }

    Matrix2D<float, 2, 2> output_3   = test_matrix_4 * test_matrix_3;
    Matrix2D<float, 2, 2> expected_3 = {103, 136, 130, 172};

    for (size_t rows = 0; rows < expected_3.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_3.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected_3(rows, columns), output_3(rows, columns));
        }
    }

    Matrix2D<float, 3, 2> output_4   = test_matrix_1 * test_matrix_3;
    Matrix2D<float, 3, 2> expected_4 = {22, 28, 49, 64, 76, 100};

    for (size_t rows = 0; rows < expected_4.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_4.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected_4(rows, columns), output_4(rows, columns));
        }
    }
}

TEST(Matrix2DTest, MatrixMultiplicationDouble) {   
    Matrix2D<double, 3, 3> test_matrix_1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
    Matrix2D<double, 3, 3> test_matrix_2 = {10, 11, 12, 13, 14, 15, 16, 17, 18};
    Matrix2D<double, 3, 3> output_1      = test_matrix_1 * test_matrix_2;
    Matrix2D<double, 3, 3> expected_1    = {84, 90, 96, 201, 216, 231, 318, 342, 366};

    for (size_t rows = 0; rows < expected_1.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_1.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected_1(rows, columns), output_1(rows, columns));
        }
    }

    Matrix2D<double, 3, 2> test_matrix_3 = {1, 2, 3, 4, 5, 6};  
    Matrix2D<double, 2, 3> test_matrix_4 = {10, 11, 12, 13, 14, 15};
    Matrix2D<double, 3, 3> output_2      = test_matrix_3 * test_matrix_4;
    Matrix2D<double, 3, 3> expected_2    = {36, 39, 42, 82, 89, 96, 128, 139, 150};

    for (size_t rows = 0; rows < expected_2.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_2.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected_2(rows, columns), output_2(rows, columns));
        }
    }

    Matrix2D<double, 2, 2> output_3   = test_matrix_4 * test_matrix_3;
    Matrix2D<double, 2, 2> expected_3 = {103, 136, 130, 172};

    for (size_t rows = 0; rows < expected_3.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_3.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected_3(rows, columns), output_3(rows, columns));
        }
    }

    Matrix2D<double, 3, 2> output_4   = test_matrix_1 * test_matrix_3;
    Matrix2D<double, 3, 2> expected_4 = {22, 28, 49, 64, 76, 100};

    for (size_t rows = 0; rows < expected_4.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_4.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected_4(rows, columns), output_4(rows, columns));
        }
    }
}

TEST(Matrix2DTest, MatrixMultipliedByScalar) {   
    Matrix2D<double, 3, 3> test_matrix = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
    Matrix2D<double, 3, 3> output_1    = 10.0 * test_matrix;
    Matrix2D<double, 3, 3> output_2    = test_matrix * 20.0;
    Matrix2D<double, 3, 3> expected_1  = {10, 20, 30, 40, 50, 60, 70, 80, 90};
    Matrix2D<double, 3, 3> expected_2  = {20, 40, 60, 80, 100, 120, 140, 160, 180};

    for (size_t rows = 0; rows < expected_1.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_1.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected_1(rows, columns), output_1(rows, columns));
            EXPECT_DOUBLE_EQ(expected_2(rows, columns), output_2(rows, columns));
        }
    }
}

TEST(Matrix2DTest, MatrixElementWiseMultiplication) {   
    Matrix2D<double, 3, 3> test_matrix_1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
    Matrix2D<double, 3, 3> test_matrix_2 = test_matrix_1;  

    Matrix2D<double, 3, 3> output      = test_matrix_1.elementWiseMultilply(test_matrix_2);
    Matrix2D<double, 3, 3> expected    = {1, 4, 9, 16, 25, 36, 49, 64, 81};

    for (size_t rows = 0; rows < expected.num_rows; rows++) {
        for (size_t columns = 0; columns < expected.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected(rows, columns), output(rows, columns));
        }
    }
}

TEST(Matrix2DTest, MatrixElementWiseDivision) {   
    Matrix2D<double, 3, 3> test_matrix_1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
    Matrix2D<double, 3, 3> test_matrix_2 = test_matrix_1;  

    Matrix2D<double, 3, 3> output      = test_matrix_1.elementWiseDivision(test_matrix_2);
    Matrix2D<double, 3, 3> expected    = {1, 1, 1, 1, 1, 1, 1, 1, 1};

    for (size_t rows = 0; rows < expected.num_rows; rows++) {
        for (size_t columns = 0; columns < expected.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected(rows, columns), output(rows, columns));
        }
    }
}


TEST(Matrix2DTest, MatrixSubtraction) {   
    Matrix2D<double, 3, 3> test_matrix_1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
    Matrix2D<double, 3, 3> test_matrix_2 = {10, 12, 14, 16, 18, 20, 22, 24, 26};
    Matrix2D<double, 3, 3> output_1      = test_matrix_1 - test_matrix_2;
    Matrix2D<double, 3, 3> expected_1    = {-9, -10, -11, -12, -13, -14, -15, -16, -17};

    for (size_t rows = 0; rows < expected_1.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_1.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected_1(rows, columns), output_1(rows, columns));
        }
    }
}

TEST(Matrix2DTest, MatrixAddition) {   
    Matrix2D<double, 3, 3> test_matrix_1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
    Matrix2D<double, 3, 3> test_matrix_2 = {10, 12, 14, 16, 18, 20, 22, 24, 26};
    Matrix2D<double, 3, 3> output_1      = test_matrix_1 + test_matrix_2;
    Matrix2D<double, 3, 3> expected_1    = {11, 14, 17, 20, 23, 26, 29, 32, 35};

    for (size_t rows = 0; rows < expected_1.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_1.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected_1(rows, columns), output_1(rows, columns));
        }
    }
}

TEST(Matrix2DTest, MatrixScalarAddition) {   
    Matrix2D<double, 3, 3> test_matrix = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
    Matrix2D<double, 3, 3> output_1    = test_matrix + 10.0;
    Matrix2D<double, 3, 3> output_2    = 10.0        + test_matrix;
    Matrix2D<double, 3, 3> expected    = {11, 12, 13, 14, 15, 16, 17, 18, 19};

    for (size_t rows = 0; rows < expected.num_rows; rows++) {
        for (size_t columns = 0; columns < expected.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected(rows, columns), output_1(rows, columns));
            EXPECT_DOUBLE_EQ(expected(rows, columns), output_2(rows, columns));
        }
    }
}

TEST(Matrix2DTest, MatrixScalarSubtraction) {   
    Matrix2D<double, 2, 2> test_matrix = {1, 2, 3, 4};  
    Matrix2D<double, 2, 2> output      = test_matrix - 10.0;
    Matrix2D<double, 2, 2> expected    = {-9, -8, -7, -6};

    for (size_t rows = 0; rows < expected.num_rows; rows++) {
        for (size_t columns = 0; columns < expected.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected(rows, columns), output(rows, columns));
        }
    }
}

TEST(Matrix2DTest, MatrixScalarDivision) {   
    Matrix2D<double, 3, 2> test_matrix = {1, 2, 3, 4, 5, 6};  
    Matrix2D<double, 3, 2> output      = test_matrix / 10.0;
    Matrix2D<double, 3, 2> expected    = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6};

    for (size_t rows = 0; rows < expected.num_rows; rows++) {
        for (size_t columns = 0; columns < expected.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected(rows, columns), output(rows, columns));
        }
    }
}

TEST(Matrix2DTest, MatrixSetRow) {   
    Matrix2D<double, 3, 3> test_matrix = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
    test_matrix.set_row(0, {10, 11, 12});
    Matrix2D<double, 3, 3> expected    = {10, 11, 12, 4, 5, 6, 7, 8, 9};

    for (size_t rows = 0; rows < expected.num_rows; rows++) {
        for (size_t columns = 0; columns < expected.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected(rows, columns), test_matrix(rows, columns));
        }
    }
}

TEST(Matrix2DTest, MatrixSetColumn) {   
    Matrix2D<double, 3, 3> test_matrix = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
    test_matrix.set_column(1, {10, 11, 12});
    Matrix2D<double, 3, 3> expected    = {1, 10, 3, 4, 11, 6, 7, 12, 9};

    for (size_t rows = 0; rows < expected.num_rows; rows++) {
        for (size_t columns = 0; columns < expected.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected(rows, columns), test_matrix(rows, columns));
        }
    }
}

TEST(Matrix2DTest, MatrixSetIdentity) {   
    Matrix2D<int, 3, 3> test_matrix = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    test_matrix.setIdentity();
    Matrix2D<int, 3, 3> expected    = {1, 0, 0, 0, 1, 0, 0, 0, 1};

    for (size_t rows = 0; rows < expected.num_rows; rows++) {
        for (size_t columns = 0; columns < expected.num_columns; columns++) {
            EXPECT_EQ(expected(rows, columns), test_matrix(rows, columns));
        }
    }
}

TEST(Matrix2DTest, MatrixSetZeros) {   
    Matrix2D<int, 3, 2> test_matrix = {1, 2, 3, 4, 5, 6};
    test_matrix.setZeros();  
    Matrix2D<int, 3, 2> expected;

    for (size_t rows = 0; rows < expected.num_rows; rows++) {
        for (size_t columns = 0; columns < expected.num_columns; columns++) {
            EXPECT_EQ(expected(rows, columns), test_matrix(rows, columns));
        }
    }
}

TEST(Matrix2DTest, MatrixInitIdentity) {   
    Matrix2D<int, 3, 3> test_matrix = identityMatrix<int, 3>();
    Matrix2D<int, 3, 3> expected    = {1, 0, 0, 0, 1, 0, 0, 0, 1};

    for (size_t rows = 0; rows < expected.num_rows; rows++) {
        for (size_t columns = 0; columns < expected.num_columns; columns++) {
            EXPECT_EQ(expected(rows, columns), test_matrix(rows, columns));
        }
    }
}

TEST(Matrix2DTest, MatrixInitZeros) {   
    Matrix2D<int, 3, 2> test_matrix = zerosMatrix<int, 3, 2>();
    Matrix2D<int, 3, 2> expected;

    for (size_t rows = 0; rows < expected.num_rows; rows++) {
        for (size_t columns = 0; columns < expected.num_columns; columns++) {
            EXPECT_EQ(expected(rows, columns), test_matrix(rows, columns));
        }
    }
}

TEST(Matrix2DTest, MatrixTranspose) {   
    Matrix2D<int, 3, 2> test_matrix_1 = {1, 2, 3, 4, 5, 6};
    Matrix2D<int, 2, 3> output_1      = test_matrix_1.transpose();
    Matrix2D<int, 2, 3> expected_1    = {1, 3, 5, 2, 4, 6};

    for (size_t rows = 0; rows < expected_1.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_1.num_columns; columns++) {
            EXPECT_EQ(expected_1(rows, columns), output_1(rows, columns));
        }
    }
    
    Matrix2D<int, 3, 3> test_matrix_2 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    test_matrix_2.inPlaceTranspose();
    Matrix2D<int, 3, 3> expected_2    = {1, 4, 7, 2, 5, 8, 3, 6, 9};

    for (size_t rows = 0; rows < expected_2.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_2.num_columns; columns++) {
            EXPECT_EQ(expected_2(rows, columns), test_matrix_2(rows, columns));
        }
    }
}

TEST(Matrix2DTest, EasyMatrixDeterminate) {
      Matrix2D<double, 10, 10> I_10 = identityMatrix<double, 10>();
      EXPECT_DOUBLE_EQ(1, I_10.det());

      Matrix2D<double, 3, 3> test_matrix_1 = {2, 0, 0,
                                              0, 3, 0,
                                              0, 0, 4};
      EXPECT_DOUBLE_EQ(24, test_matrix_1.det());

      Matrix2D<double, 3, 3> test_matrix_2 = {1, 2, 3,
                                              0, 4, 5,
                                              0, 0, 6};
      EXPECT_DOUBLE_EQ(24, test_matrix_2.det());

      Matrix2D<double, 2, 2> test_matrix_3 = {2, 4,
                                              1, 2};
      EXPECT_DOUBLE_EQ(0, test_matrix_3.det());

      Matrix2D<double, 2, 2> test_matrix_4 = {0, 1,
                                              1, 0};
      EXPECT_DOUBLE_EQ(-1, test_matrix_4.det());

      Matrix2D<double, 3, 3> test_matrix_5 = {0, 1, 0,
                                              0, 0, 1,
                                              1, 0, 0};
      EXPECT_DOUBLE_EQ(1.0, test_matrix_5.det());

      Matrix2D<double, 3, 3> test_matrix_6 = {1,2,3,
                                              4,5,6,
                                              1,2,3};
      EXPECT_DOUBLE_EQ(0.0, test_matrix_6.det());
}

TEST(Matrix2DTest, MediumMatrixDeterminate) {
    Matrix2D<double, 3, 3> M1 = { 7, -3,  2,
                                  4,  5, -6,
                                  1,  8,  9};
    EXPECT_DOUBLE_EQ(M1.det(), 831.0);

    Matrix2D<double, 3, 3> M2 = {-12,  7,  5,
                                  3, -9, 14,
                                  6,  6,  8};
    EXPECT_DOUBLE_EQ(M2.det(), 2652.0);

    Matrix2D<double, 4, 4> M3 = { 2,  5,  3,  7,
                                 -1,  4,  0,  2,
                                  3, -2,  6,  4,
                                  1,  0,  5, -3};
    EXPECT_DOUBLE_EQ(M3.det(), -292.0);

    Matrix2D<double, 4, 4> M4 = {10, -7,  3,  4,
                                  2,  0, -5,  6,
                                 -3,  8,  9,  1,
                                  5, -2,  7, -6};
    EXPECT_DOUBLE_EQ(M4.det(), -3191.0);


    Matrix2D<double, 4, 4> M5 = { 6,  1, -2,  8,
                                  3,  5,  7, -4,
                                 -1,  9,  2,  6,
                                  4, -3,  8,  0};
    EXPECT_DOUBLE_EQ(M5.det(), -5432.0);

    Matrix2D<double, 4, 4> M6 = {2, 5, 7, 1,
                                 0, 3, 6, 4,
                                 0, 0,-2, 8,
                                 0, 0, 0, 9};
    EXPECT_DOUBLE_EQ(M6.det(), -108.0);

    Matrix2D<double, 4, 4> M7 = {1, 2, 0, 0,
                                 3, 4, 0, 0,
                                 0, 0, 5, 6,
                                 0, 0, 7, 8};
    EXPECT_DOUBLE_EQ(M7.det(), 4.0);
}

TEST(Matrix2DTest, PrecisionMatrixDeterminate) {
    Matrix2D<double, 3, 3> A1 = {1, 1, 1,
                                 1, 1 + 1e-8, 1,
                                 1, 1, 1 + 1e-8};
    EXPECT_NEAR(1e-16, A1.det(), 1e-18);

    Matrix2D<double, 3, 3> A2 = {1, 1, 1, 
                                 1, 1 + 1e-12, 1,
                                 1, 1, 1 + 1e-12};
    EXPECT_NEAR(1e-24, A2.det(), 1e-26);

    Matrix2D<double, 3, 3> A3 = {1, 1, 1,
                                 1, 1 + 1e-14, 1,
                                 1, 1, 1 + 1e-14};
    EXPECT_NEAR(1e-28, A3.det(), 1e-29);

    Matrix2D<double, 3, 3> A4 = {1, 1, 1,
                                 1, 1 + 1e-16, 1,
                                 1, 1, 1 + 1e-16};
    EXPECT_DOUBLE_EQ(0, A4.det());
}

TEST(Matrix2DTest, HugeMatrixDeterminate) {
    const int size = 50; 
    Matrix2D<double, size, size> big_test_matrix;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i == j) {
                big_test_matrix(i,j) = 1e-12;  // diagonal
            } else if (j > i) {
                big_test_matrix(i,j) = 1.0;    // above diagonal
            } else {
                big_test_matrix(i,j) = 0.0;    // below diagonal
            }
        }
    } 

    double expected_det = std::pow(1e-12, size);  // (1e-12)^n
    EXPECT_DOUBLE_EQ(expected_det, big_test_matrix.det());
}

template <int N>
void runRandomMultiplicationCheck(std::mt19937& rng) {
    std::uniform_real_distribution<double> dist(-10.0, 10.0);

    Matrix2D<double, N, N> A, B, C;
    Eigen::Matrix<double, N, N> eigA, eigB, eigC;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A(i, j) = dist(rng);
            B(i, j) = dist(rng);
            eigA(i, j) = A(i, j);
            eigB(i, j) = B(i, j);
        }
    }

    C = A * B;
    eigC = eigA * eigB;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            EXPECT_NEAR(C(i, j), eigC(i, j), 1e-6);
        }
    }
}

TEST(Matrix2DTest, RandomMultiplicationTestAgainstEigen) {
    std::mt19937 rng(0);
    int num_tests = 100;
    int min_mat_size = 2; 
    int max_mat_size = 6;

    for (int test = 0; test < num_tests; test++) {
        int random_mat_size = min_mat_size + (rng() % (max_mat_size-1)); 

        switch (random_mat_size) {
            case 2: runRandomMultiplicationCheck<2>(rng); break;
            case 3: runRandomMultiplicationCheck<3>(rng); break;
            case 4: runRandomMultiplicationCheck<4>(rng); break;
            case 5: runRandomMultiplicationCheck<5>(rng); break;
            case 6: runRandomMultiplicationCheck<6>(rng); break;
        }
    }
}

template <int N>
void runRandomAdditionCheck(std::mt19937& rng) {
    std::uniform_real_distribution<double> dist(-10.0, 10.0);

    Matrix2D<double, N, N> A, B, C;
    Eigen::Matrix<double, N, N> eigA, eigB, eigC;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A(i, j) = dist(rng);
            B(i, j) = dist(rng);
            eigA(i, j) = A(i, j);
            eigB(i, j) = B(i, j);
        }
    }

    C = A + B;
    eigC = eigA + eigB;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            EXPECT_NEAR(C(i, j), eigC(i, j), 1e-6);
        }
    }
}

TEST(Matrix2DTest, RandomAdditionTestAgainstEigen) {
    std::mt19937 rng(0);
    int num_tests = 100;
    int min_mat_size = 2; 
    int max_mat_size = 6;

    for (int test = 0; test < num_tests; test++) {
        int random_mat_size = min_mat_size + (rng() % (max_mat_size-1)); 

        switch (random_mat_size) {
            case 2: runRandomAdditionCheck<2>(rng); break;
            case 3: runRandomAdditionCheck<3>(rng); break;
            case 4: runRandomAdditionCheck<4>(rng); break;
            case 5: runRandomAdditionCheck<5>(rng); break;
            case 6: runRandomAdditionCheck<6>(rng); break;
        }
    }
}

template <int N>
void runRandomSubtractionCheck(std::mt19937& rng) {
    std::uniform_real_distribution<double> dist(-10.0, 10.0);

    Matrix2D<double, N, N> A, B, C;
    Eigen::Matrix<double, N, N> eigA, eigB, eigC;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A(i, j) = dist(rng);
            B(i, j) = dist(rng);
            eigA(i, j) = A(i, j);
            eigB(i, j) = B(i, j);
        }
    }

    C = A - B;
    eigC = eigA - eigB;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            EXPECT_NEAR(C(i, j), eigC(i, j), 1e-6);
        }
    }
}

TEST(Matrix2DTest, RandomSubtractionTestAgainstEigen) {
    std::mt19937 rng(0);
    int num_tests = 100;
    int min_mat_size = 2; 
    int max_mat_size = 6;

    for (int test = 0; test < num_tests; test++) {
        int random_mat_size = min_mat_size + (rng() % (max_mat_size-1)); 

        switch (random_mat_size) {
            case 2: runRandomSubtractionCheck<2>(rng); break;
            case 3: runRandomSubtractionCheck<3>(rng); break;
            case 4: runRandomSubtractionCheck<4>(rng); break;
            case 5: runRandomSubtractionCheck<5>(rng); break;
            case 6: runRandomSubtractionCheck<6>(rng); break;
        }
    }
}


template <int N>
void runRandomDeterminantCheck(std::mt19937& rng) {
    std::uniform_real_distribution<double> dist(-10.0, 10.0);

    Matrix2D<double, N, N> A;
    Eigen::Matrix<double, N, N> eigA;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A(i, j) = dist(rng);
            eigA(i, j) = A(i, j);
        }
    }

    double det_expected = eigA.determinant();
    double det_actual   = static_cast<double>(A.det());

    EXPECT_NEAR(det_expected, det_actual, 1e-6);
}

TEST(Matrix2DTest, RandomDeterminantTestAgainstEigen) {
    std::mt19937 rng(0);
    int num_tests = 100;
    int min_mat_size = 2; 
    int max_mat_size = 6;

    for (int test = 0; test < num_tests; test++) {
        int random_mat_size = min_mat_size + (rng() % (max_mat_size-1)); 

        switch (random_mat_size) {
            case 2: runRandomDeterminantCheck<2>(rng); break;
            case 3: runRandomDeterminantCheck<3>(rng); break;
            case 4: runRandomDeterminantCheck<4>(rng); break;
            case 5: runRandomDeterminantCheck<5>(rng); break;
            case 6: runRandomDeterminantCheck<6>(rng); break;
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
