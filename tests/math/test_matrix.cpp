#include <gtest/gtest.h>
#include "../src/math/matrix/matrix.hpp"
#include <Eigen/Dense>
#include <random>
#include <limits> 

// Test constructor and element access
TEST(matrixTest, ConstructorAndAccessTest) {
    matrix<int, 2, 2> mat = {1, 2, 3, 4};
    EXPECT_EQ(mat(0, 0), 1);
    EXPECT_EQ(mat(0, 1), 2);
    EXPECT_EQ(mat(1, 0), 3);
    EXPECT_EQ(mat(1, 1), 4);

    mat(1,0) = 100; 
    EXPECT_EQ(mat(1,0), 100);
}

TEST(matrixTest, MatrixMultiplicationInt) {   
    matrix<int, 3, 3> test_matrix_1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
    matrix<int, 3, 3> test_matrix_2 = {10, 11, 12, 13, 14, 15, 16, 17, 18};
    matrix<int, 3, 3> output_1      = test_matrix_1 * test_matrix_2;
    matrix<int, 3, 3> expected_1    = {84, 90, 96, 201, 216, 231, 318, 342, 366};

    for (size_t rows = 0; rows < expected_1.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_1.num_columns; columns++) {
            EXPECT_EQ(expected_1(rows, columns), output_1(rows, columns));
        }
    }

    matrix<int, 3, 2> test_matrix_3 = {1, 2, 3, 4, 5, 6};  
    matrix<int, 2, 3> test_matrix_4 = {10, 11, 12, 13, 14, 15};
    matrix<int, 3, 3> output_2      = test_matrix_3 * test_matrix_4;
    matrix<int, 3, 3> expected_2    = {36, 39, 42, 82, 89, 96, 128, 139, 150};

    for (size_t rows = 0; rows < expected_2.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_2.num_columns; columns++) {
            EXPECT_EQ(expected_2(rows, columns), output_2(rows, columns));
        }
    }

    matrix<int, 2, 2> output_3   = test_matrix_4 * test_matrix_3;
    matrix<int, 2, 2> expected_3 = {103, 136, 130, 172};

    for (size_t rows = 0; rows < expected_3.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_3.num_columns; columns++) {
            EXPECT_EQ(expected_3(rows, columns), output_3(rows, columns));
        }
    }

    matrix<int, 3, 2> output_4   = test_matrix_1 * test_matrix_3;
    matrix<int, 3, 2> expected_4 = {22, 28, 49, 64, 76, 100};

    for (size_t rows = 0; rows < expected_4.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_4.num_columns; columns++) {
            EXPECT_EQ(expected_4(rows, columns), output_4(rows, columns));
        }
    }
}

TEST(matrixTest, MatrixMultiplicationFloat) {   
    matrix<float, 3, 3> test_matrix_1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
    matrix<float, 3, 3> test_matrix_2 = {10, 11, 12, 13, 14, 15, 16, 17, 18};
    matrix<float, 3, 3> output_1      = test_matrix_1 * test_matrix_2;
    matrix<float, 3, 3> expected_1    = {84, 90, 96, 201, 216, 231, 318, 342, 366};

    for (size_t rows = 0; rows < expected_1.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_1.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected_1(rows, columns), output_1(rows, columns));
        }
    }

    matrix<float, 3, 2> test_matrix_3 = {1, 2, 3, 4, 5, 6};  
    matrix<float, 2, 3> test_matrix_4 = {10, 11, 12, 13, 14, 15};
    matrix<float, 3, 3> output_2      = test_matrix_3 * test_matrix_4;
    matrix<float, 3, 3> expected_2    = {36, 39, 42, 82, 89, 96, 128, 139, 150};

    for (size_t rows = 0; rows < expected_2.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_2.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected_2(rows, columns), output_2(rows, columns));
        }
    }

    matrix<float, 2, 2> output_3   = test_matrix_4 * test_matrix_3;
    matrix<float, 2, 2> expected_3 = {103, 136, 130, 172};

    for (size_t rows = 0; rows < expected_3.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_3.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected_3(rows, columns), output_3(rows, columns));
        }
    }

    matrix<float, 3, 2> output_4   = test_matrix_1 * test_matrix_3;
    matrix<float, 3, 2> expected_4 = {22, 28, 49, 64, 76, 100};

    for (size_t rows = 0; rows < expected_4.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_4.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected_4(rows, columns), output_4(rows, columns));
        }
    }
}

TEST(matrixTest, MatrixMultiplicationDouble) {   
    matrix<double, 3, 3> test_matrix_1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
    matrix<double, 3, 3> test_matrix_2 = {10, 11, 12, 13, 14, 15, 16, 17, 18};
    matrix<double, 3, 3> output_1      = test_matrix_1 * test_matrix_2;
    matrix<double, 3, 3> expected_1    = {84, 90, 96, 201, 216, 231, 318, 342, 366};

    for (size_t rows = 0; rows < expected_1.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_1.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected_1(rows, columns), output_1(rows, columns));
        }
    }

    matrix<double, 3, 2> test_matrix_3 = {1, 2, 3, 4, 5, 6};  
    matrix<double, 2, 3> test_matrix_4 = {10, 11, 12, 13, 14, 15};
    matrix<double, 3, 3> output_2      = test_matrix_3 * test_matrix_4;
    matrix<double, 3, 3> expected_2    = {36, 39, 42, 82, 89, 96, 128, 139, 150};

    for (size_t rows = 0; rows < expected_2.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_2.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected_2(rows, columns), output_2(rows, columns));
        }
    }

    matrix<double, 2, 2> output_3   = test_matrix_4 * test_matrix_3;
    matrix<double, 2, 2> expected_3 = {103, 136, 130, 172};

    for (size_t rows = 0; rows < expected_3.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_3.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected_3(rows, columns), output_3(rows, columns));
        }
    }

    matrix<double, 3, 2> output_4   = test_matrix_1 * test_matrix_3;
    matrix<double, 3, 2> expected_4 = {22, 28, 49, 64, 76, 100};

    for (size_t rows = 0; rows < expected_4.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_4.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected_4(rows, columns), output_4(rows, columns));
        }
    }
}

TEST(matrixTest, MatrixMultipliedByScalar) {   
    matrix<double, 3, 3> test_matrix = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
    matrix<double, 3, 3> output_1    = 10.0 * test_matrix;
    matrix<double, 3, 3> output_2    = test_matrix * 20.0;
    matrix<double, 3, 3> expected_1  = {10, 20, 30, 40, 50, 60, 70, 80, 90};
    matrix<double, 3, 3> expected_2  = {20, 40, 60, 80, 100, 120, 140, 160, 180};

    for (size_t rows = 0; rows < expected_1.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_1.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected_1(rows, columns), output_1(rows, columns));
            EXPECT_DOUBLE_EQ(expected_2(rows, columns), output_2(rows, columns));
        }
    }
}

TEST(matrixTest, MatrixElementWiseMultiplication) {   
    matrix<double, 3, 3> test_matrix_1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
    matrix<double, 3, 3> test_matrix_2 = test_matrix_1;  

    matrix<double, 3, 3> output      = test_matrix_1.elementWiseMultiply(test_matrix_2);
    matrix<double, 3, 3> expected    = {1, 4, 9, 16, 25, 36, 49, 64, 81};

    for (size_t rows = 0; rows < expected.num_rows; rows++) {
        for (size_t columns = 0; columns < expected.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected(rows, columns), output(rows, columns));
        }
    }
}

TEST(matrixTest, MatrixElementWiseDivision) {   
    matrix<double, 3, 3> test_matrix_1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
    matrix<double, 3, 3> test_matrix_2 = test_matrix_1;  

    matrix<double, 3, 3> output      = test_matrix_1.elementWiseDivision(test_matrix_2);
    matrix<double, 3, 3> expected    = {1, 1, 1, 1, 1, 1, 1, 1, 1};

    for (size_t rows = 0; rows < expected.num_rows; rows++) {
        for (size_t columns = 0; columns < expected.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected(rows, columns), output(rows, columns));
        }
    }
}


TEST(matrixTest, MatrixSubtraction) {   
    matrix<double, 3, 3> test_matrix_1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
    matrix<double, 3, 3> test_matrix_2 = {10, 12, 14, 16, 18, 20, 22, 24, 26};
    matrix<double, 3, 3> output_1      = test_matrix_1 - test_matrix_2;
    matrix<double, 3, 3> expected_1    = {-9, -10, -11, -12, -13, -14, -15, -16, -17};

    for (size_t rows = 0; rows < expected_1.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_1.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected_1(rows, columns), output_1(rows, columns));
        }
    }
}

TEST(matrixTest, MatrixAddition) {   
    matrix<double, 3, 3> test_matrix_1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
    matrix<double, 3, 3> test_matrix_2 = {10, 12, 14, 16, 18, 20, 22, 24, 26};
    matrix<double, 3, 3> output_1      = test_matrix_1 + test_matrix_2;
    matrix<double, 3, 3> expected_1    = {11, 14, 17, 20, 23, 26, 29, 32, 35};

    for (size_t rows = 0; rows < expected_1.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_1.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected_1(rows, columns), output_1(rows, columns));
        }
    }
}

TEST(matrixTest, MatrixScalarAddition) {   
    matrix<double, 3, 3> test_matrix = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
    matrix<double, 3, 3> output_1    = test_matrix + 10.0;
    matrix<double, 3, 3> output_2    = 10.0        + test_matrix;
    matrix<double, 3, 3> expected    = {11, 12, 13, 14, 15, 16, 17, 18, 19};

    for (size_t rows = 0; rows < expected.num_rows; rows++) {
        for (size_t columns = 0; columns < expected.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected(rows, columns), output_1(rows, columns));
            EXPECT_DOUBLE_EQ(expected(rows, columns), output_2(rows, columns));
        }
    }
}

TEST(matrixTest, MatrixScalarSubtraction) {   
    matrix<double, 2, 2> test_matrix = {1, 2, 3, 4};  
    matrix<double, 2, 2> output      = test_matrix - 10.0;
    matrix<double, 2, 2> expected    = {-9, -8, -7, -6};

    for (size_t rows = 0; rows < expected.num_rows; rows++) {
        for (size_t columns = 0; columns < expected.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected(rows, columns), output(rows, columns));
        }
    }
}

TEST(matrixTest, MatrixScalarDivision) {   
    matrix<double, 3, 2> test_matrix = {1, 2, 3, 4, 5, 6};  
    matrix<double, 3, 2> output      = test_matrix / 10.0;
    matrix<double, 3, 2> expected    = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6};

    for (size_t rows = 0; rows < expected.num_rows; rows++) {
        for (size_t columns = 0; columns < expected.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected(rows, columns), output(rows, columns));
        }
    }
}

TEST(matrixTest, MatrixSetRow) {   
    matrix<double, 3, 3> test_matrix = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
    test_matrix.set_row(0, {10, 11, 12});
    matrix<double, 3, 3> expected    = {10, 11, 12, 4, 5, 6, 7, 8, 9};

    for (size_t rows = 0; rows < expected.num_rows; rows++) {
        for (size_t columns = 0; columns < expected.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected(rows, columns), test_matrix(rows, columns));
        }
    }
}

TEST(matrixTest, MatrixSetColumn) {   
    matrix<double, 3, 3> test_matrix = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
    test_matrix.set_column(1, {10, 11, 12});
    matrix<double, 3, 3> expected    = {1, 10, 3, 4, 11, 6, 7, 12, 9};

    for (size_t rows = 0; rows < expected.num_rows; rows++) {
        for (size_t columns = 0; columns < expected.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected(rows, columns), test_matrix(rows, columns));
        }
    }
}

TEST(matrixTest, MatrixSetIdentity) {   
    matrix<int, 3, 3> test_matrix = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    test_matrix.setIdentity();
    matrix<int, 3, 3> expected    = {1, 0, 0, 0, 1, 0, 0, 0, 1};

    for (size_t rows = 0; rows < expected.num_rows; rows++) {
        for (size_t columns = 0; columns < expected.num_columns; columns++) {
            EXPECT_EQ(expected(rows, columns), test_matrix(rows, columns));
        }
    }
}

TEST(matrixTest, MatrixSetZeros) {   
    matrix<int, 3, 2> test_matrix = {1, 2, 3, 4, 5, 6};
    test_matrix.setZeros();  
    matrix<int, 3, 2> expected;

    for (size_t rows = 0; rows < expected.num_rows; rows++) {
        for (size_t columns = 0; columns < expected.num_columns; columns++) {
            EXPECT_EQ(expected(rows, columns), test_matrix(rows, columns));
        }
    }
}

TEST(matrixTest, MatrixInitIdentity) {   
    matrix<int, 3, 3> test_matrix = identityMatrix<int, 3>();
    matrix<int, 3, 3> expected    = {1, 0, 0, 0, 1, 0, 0, 0, 1};

    for (size_t rows = 0; rows < expected.num_rows; rows++) {
        for (size_t columns = 0; columns < expected.num_columns; columns++) {
            EXPECT_EQ(expected(rows, columns), test_matrix(rows, columns));
        }
    }
}

TEST(matrixTest, MatrixInitZeros) {   
    matrix<int, 3, 2> test_matrix = zerosMatrix<int, 3, 2>();
    matrix<int, 3, 2> expected;

    for (size_t rows = 0; rows < expected.num_rows; rows++) {
        for (size_t columns = 0; columns < expected.num_columns; columns++) {
            EXPECT_EQ(expected(rows, columns), test_matrix(rows, columns));
        }
    }
}

TEST(matrixTest, MatrixTranspose) {   
    matrix<int, 3, 2> test_matrix_1 = {1, 2, 3, 4, 5, 6};
    matrix<int, 2, 3> output_1      = test_matrix_1.transpose();
    matrix<int, 2, 3> expected_1    = {1, 3, 5, 2, 4, 6};

    for (size_t rows = 0; rows < expected_1.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_1.num_columns; columns++) {
            EXPECT_EQ(expected_1(rows, columns), output_1(rows, columns));
        }
    }
    
    matrix<int, 3, 3> test_matrix_2 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    test_matrix_2.inPlaceTranspose();
    matrix<int, 3, 3> expected_2    = {1, 4, 7, 2, 5, 8, 3, 6, 9};

    for (size_t rows = 0; rows < expected_2.num_rows; rows++) {
        for (size_t columns = 0; columns < expected_2.num_columns; columns++) {
            EXPECT_EQ(expected_2(rows, columns), test_matrix_2(rows, columns));
        }
    }
}

TEST(matrixTest, EasyMatrixDeterminate) {
      matrix<double, 10, 10> I_10 = identityMatrix<double, 10>();
      EXPECT_DOUBLE_EQ(1, I_10.det());

      matrix<double, 3, 3> test_matrix_1 = {2, 0, 0,
                                              0, 3, 0,
                                              0, 0, 4};
      EXPECT_DOUBLE_EQ(24, test_matrix_1.det());

      matrix<double, 3, 3> test_matrix_2 = {1, 2, 3,
                                              0, 4, 5,
                                              0, 0, 6};
      EXPECT_DOUBLE_EQ(24, test_matrix_2.det());

      matrix<double, 2, 2> test_matrix_3 = {2, 4,
                                              1, 2};
      EXPECT_DOUBLE_EQ(0, test_matrix_3.det());

      matrix<double, 2, 2> test_matrix_4 = {0, 1,
                                              1, 0};
      EXPECT_DOUBLE_EQ(-1, test_matrix_4.det());

      matrix<double, 3, 3> test_matrix_5 = {0, 1, 0,
                                              0, 0, 1,
                                              1, 0, 0};
      EXPECT_DOUBLE_EQ(1.0, test_matrix_5.det());

      matrix<double, 3, 3> test_matrix_6 = {1,2,3,
                                              4,5,6,
                                              1,2,3};
      EXPECT_DOUBLE_EQ(0.0, test_matrix_6.det());
}

TEST(matrixTest, MediumMatrixDeterminate) {
    matrix<double, 3, 3> M1 = { 7, -3,  2,
                                  4,  5, -6,
                                  1,  8,  9};
    EXPECT_DOUBLE_EQ(M1.det(), 831.0);

    matrix<double, 3, 3> M2 = {-12,  7,  5,
                                  3, -9, 14,
                                  6,  6,  8};
    EXPECT_DOUBLE_EQ(M2.det(), 2652.0);

    matrix<double, 4, 4> M3 = { 2,  5,  3,  7,
                                 -1,  4,  0,  2,
                                  3, -2,  6,  4,
                                  1,  0,  5, -3};
    EXPECT_DOUBLE_EQ(M3.det(), -292.0);

    matrix<double, 4, 4> M4 = {10, -7,  3,  4,
                                  2,  0, -5,  6,
                                 -3,  8,  9,  1,
                                  5, -2,  7, -6};
    EXPECT_DOUBLE_EQ(M4.det(), -3191.0);


    matrix<double, 4, 4> M5 = { 6,  1, -2,  8,
                                  3,  5,  7, -4,
                                 -1,  9,  2,  6,
                                  4, -3,  8,  0};
    EXPECT_DOUBLE_EQ(M5.det(), -5432.0);

    matrix<double, 4, 4> M6 = {2, 5, 7, 1,
                                 0, 3, 6, 4,
                                 0, 0,-2, 8,
                                 0, 0, 0, 9};
    EXPECT_DOUBLE_EQ(M6.det(), -108.0);

    matrix<double, 4, 4> M7 = {1, 2, 0, 0,
                                 3, 4, 0, 0,
                                 0, 0, 5, 6,
                                 0, 0, 7, 8};
    EXPECT_DOUBLE_EQ(M7.det(), 4.0);
}

TEST(matrixTest, PrecisionMatrixDeterminate) {
    matrix<double, 3, 3> A1 = {1, 1, 1,
                                 1, 1 + 1e-8, 1,
                                 1, 1, 1 + 1e-8};
    EXPECT_NEAR(1e-16, A1.det(), 1e-18);

    matrix<double, 3, 3> A2 = {1, 1, 1, 
                                 1, 1 + 1e-12, 1,
                                 1, 1, 1 + 1e-12};
    EXPECT_NEAR(1e-24, A2.det(), 1e-26);

    matrix<double, 3, 3> A3 = {1, 1, 1,
                                 1, 1 + 1e-14, 1,
                                 1, 1, 1 + 1e-14};
    EXPECT_NEAR(1e-28, A3.det(), 1e-29);

    matrix<double, 3, 3> A4 = {1, 1, 1,
                                 1, 1 + 1e-16, 1,
                                 1, 1, 1 + 1e-16};
    EXPECT_DOUBLE_EQ(0, A4.det());
}

TEST(matrixTest, HugeMatrixDeterminate) {
    const int size = 50; 
    matrix<double, size, size> big_test_matrix;

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

TEST(matrixTest, EasyMatrixInverse) {
    matrix<double, 3,3> A = {2,1,1, 1,3,2, 1,0,0};
    matrix<double, 3,3> A_inv = A.inv();
    matrix<double, 3,3> A_inv_expected{0, 0, 1, -2, 1, 3,  3, -1, -5};

    matrix<double, 2,2> B = {4,7, 2,6};
    matrix<double, 2,2> B_inv = B.inv();
    matrix<double, 2,2> B_inv_expected{0.6, -0.7, -0.2, 0.4};

    matrix<double, 3,3> C = {1,2,3, 0,1,4, 5,6,0};
    matrix<double, 3,3> C_inv = C.inv();
    matrix<double, 3,3> C_inv_expected{-24, 18, 5, 20, -15, -4, -5, 4, 1};

    matrix<double, 2,2> D = {1,2, 3,4};
    matrix<double, 2,2> D_inv = D.inv();
    matrix<double, 2,2> D_inv_expected{-2,1, 1.5,-0.5};

    matrix<double, 3,3> E = {3,0,2, 2,0,-2, 0,1,1};
    matrix<double, 3,3> E_inv = E.inv();
    matrix<double, 3,3> E_inv_expected{0.2,0.2,0, -0.2,0.3,1, 0.2,-0.3,0};


    for (std::size_t row = 0; row < 3; row++) {
        for (std::size_t column = 0; column < 3; column++) {
            EXPECT_NEAR(A_inv(row, column), A_inv_expected(row, column), 1e-12);
            EXPECT_NEAR(C_inv(row, column), C_inv_expected(row, column), 1e-12);
            EXPECT_NEAR(E_inv(row, column), E_inv_expected(row, column), 1e-12);
        }
    }

    for (std::size_t row = 0; row < 2; row++) {
        for (std::size_t column = 0; column < 2; column++) {
            EXPECT_NEAR(B_inv(row, column), B_inv_expected(row, column), 1e-12);
            EXPECT_NEAR(D_inv(row, column), D_inv_expected(row, column), 1e-12);
        }
    }
}

TEST(matrix, PrecisionMatrixInverse) {

    matrix<double, 3,3> K = {1e-12, 0, 0,  0, 1, 0,  0, 0, 1e12};
    matrix<double, 3,3> K_inv = K.inv();
    matrix<double, 3,3> K_inv_expected = {1e12, 0, 0,  0, 1, 0,  0, 0, 1e-12};

    matrix<double, 4,4> L = {1e-12, 0, 0, 0,  0, 1e12, 0, 0,  0, 0, 3, -1,  0, 0, -1, 2};
    matrix<double, 4,4> L_inv = L.inv();
    matrix<double, 4,4> L_inv_expected = {1e12, 0, 0, 0,  0, 1e-12, 0, 0,  0, 0, 0.4, 0.2,  0, 0, 0.2, 0.6};

    matrix<double, 2,2> M = {1e-6, 0,  0, 1e12};
    matrix<double, 2,2> M_inv = M.inv();
    matrix<double, 2,2> M_inv_expected = {1e6, 0,  0, 1e-12};

    matrix<double, 3,3> N = {1e12, 1, 1,  1, 1e-12, 1,  1, 1, 1};
    matrix<double, 3,3> N_inv = N.inv();
    matrix<double, 3,3> N_inv_expected = {1.000000000001e-12, 0.0, -1.000000000001e-12,
                                            0.0, -1.000000000001,  1.000000000001,
                                        -1.000000000001e-12,   1.000000000001, 0.0};

    matrix<double, 2,2> O = {0, 1e12,  -1e-12, 0};
    matrix<double, 2,2> O_inv = O.inv();
    matrix<double, 2,2> O_inv_expected = {0, -1e12,  1e-12, 0};

    for (std::size_t row = 0; row < 4; row++) {
        for (std::size_t column = 0; column < 4; column++) {
            EXPECT_NEAR(L_inv(row, column), L_inv_expected(row, column), 1e-12);
        }
    }

    for (std::size_t row = 0; row < 3; row++) {
        for (std::size_t column = 0; column < 3; column++) {
            EXPECT_NEAR(K_inv(row, column), K_inv_expected(row, column), 1e-12);
            EXPECT_NEAR(N_inv(row, column), N_inv_expected(row, column), 1e-12);
        }
    }

    for (std::size_t row = 0; row < 2; row++) {
        for (std::size_t column = 0; column < 2; column++) {
            EXPECT_NEAR(M_inv(row, column), M_inv_expected(row, column), 1e-12);
            EXPECT_NEAR(O_inv(row, column), O_inv_expected(row, column), 1e-12);
        }
    }
}

TEST(matrix, SimpleMatrixToPower) {
    matrix<double, 3, 3> I = identityMatrix<double, 3>();
    matrix<double, 3, 3> test_matrix_1 = I.pow(10);
    
    matrix<double, 3, 3> generic_matrix = {2,3,4, 11,15,20, 4,5,6};

    matrix<double, 3, 3> test_matrix_2 = generic_matrix;
    test_matrix_2 = test_matrix_2.pow(0);

    matrix<double, 3, 3> test_matrix_3 = generic_matrix.pow(3);
    matrix<double, 3, 3> test_matrix_3_expected = {1255,1684,2184, 6328,8491,11012, 2069,2776,3600};

    matrix<double, 3, 3> test_matrix_4 = generic_matrix;
    test_matrix_4 = test_matrix_4.pow(-2);
    matrix<double, 3, 3> test_matrix_4_expected = {32.0,-7.0,2.0, -54.0,13.0,-7.0, 23.25,-6.0,4.25};

    matrix<double, 3, 3> test_matrix_5 = generic_matrix;
    test_matrix_5 = test_matrix_5.pow(-1);
    matrix<double, 3, 3> test_matrix_5_expected = generic_matrix.inv();

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_DOUBLE_EQ(test_matrix_1(i, j), I(i, j));
            EXPECT_DOUBLE_EQ(test_matrix_2(i, j), I(i, j));
            EXPECT_DOUBLE_EQ(test_matrix_3(i, j), test_matrix_3_expected(i, j));
            EXPECT_DOUBLE_EQ(test_matrix_4(i, j), test_matrix_4_expected(i, j));
            EXPECT_DOUBLE_EQ(test_matrix_5(i, j), test_matrix_5_expected(i, j));
        }
    }
}

TEST(matrix, SimpleOrderOfOperation) {
    matrix<double, 2,2> A = {1,2, 3,4};
    matrix<double, 2,2> B = {5,6, 7,8};
    matrix<double, 2,2> C = {9,10, 11,12};
    
    matrix<double, 2,2> output_1 = C * B * A;
    matrix<double, 2,2> expected_output_1 = {517,766, 625,926};

    matrix<double, 2,2> output_2 = (C*B)*A;
    matrix<double, 2,2> expected_output_2 = {517,766, 625,926};

    matrix<double, 2,2> output_3 = (B*A).pow(2);
    matrix<double, 2,2> expected_output_3 = {1583,2346, 2139,3170};

    matrix<double, 2,2> output_4 = A.transpose() + A.inv() + A.pow(2);
    matrix<double, 2,2> expected_output_4 = {6,14, 18.5,25.5};

    matrix<double, 2,2> output_5 = A.transpose() * A.inv() * A.pow(2);
    matrix<double, 2,2> expected_output_5 = {10,14, 14,20};

    matrix<double, 2,2> output_6 = A * B + C;
    matrix<double, 2,2> expected_output_6 = {28,32, 54,62};

    matrix<double, 2,2> output_7 = A - B + C;
    matrix<double, 2,2> expected_output_7 = {5,6, 7,8};

    for (std::size_t row = 0; row < output_1.num_rows; row++) {
        for (std::size_t column = 0; column < output_1.num_columns; column++) {
            EXPECT_DOUBLE_EQ(output_1(row, column), expected_output_1(row, column));
            EXPECT_DOUBLE_EQ(output_2(row, column), expected_output_2(row, column));
            EXPECT_DOUBLE_EQ(output_3(row, column), expected_output_3(row, column));
            EXPECT_DOUBLE_EQ(output_4(row, column), expected_output_4(row, column));
            EXPECT_DOUBLE_EQ(output_5(row, column), expected_output_5(row, column));
            EXPECT_DOUBLE_EQ(output_6(row, column), expected_output_6(row, column));
            EXPECT_DOUBLE_EQ(output_7(row, column), expected_output_7(row, column));
        }
    }
}

template <int N>
void runRandomMultiplicationCheck(std::mt19937& rng) {
    std::uniform_real_distribution<double> dist(-10.0, 10.0);

    matrix<double, N, N> A, B, C;
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

TEST(matrixTest, RandomMultiplicationTestAgainstEigen) {
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

    matrix<double, N, N> A, B, C;
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

TEST(matrixTest, RandomAdditionTestAgainstEigen) {
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

    matrix<double, N, N> A, B, C;
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

TEST(matrixTest, RandomSubtractionTestAgainstEigen) {
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

    matrix<double, N, N> A;
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

TEST(matrixTest, RandomDeterminantTestAgainstEigen) {
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

template <int N>
void runRandomInverseCheck(std::mt19937& rng) {
    std::uniform_real_distribution<double> dist(-5, 5);
    std::uniform_int_distribution<int> random_power(-5, 5);

    matrix<double, N, N> A, A_pow, A_expected;
    A_expected.setIdentity();

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A(i, j) = dist(rng);
        }
    }

    int exponent = random_power(rng);
    A_pow        = A.pow(exponent);

    if (exponent < 0) {
        A = A.inv();
    }

    for (int i = 0; i < std::abs(exponent); i++) {
        A_expected *= A;
    }

    for (std::size_t row = 0; row < N; row++) {
        for (std::size_t column = 0; column < N; column++) {
            EXPECT_NEAR(A_pow(row, column), A_expected(row, column), 1e-6);
        }
    }
}

TEST(matrixTest, RandomInverseTest) {
    std::mt19937 rng(0);
    int num_tests = 100;
    int min_mat_size = 2; 
    int max_mat_size = 6;

    for (int test = 0; test < num_tests; test++) {
        int random_mat_size = min_mat_size + (rng() % (max_mat_size-1)); 

        switch (random_mat_size) {
            case 2: runRandomInverseCheck<2>(rng); break;
            case 3: runRandomInverseCheck<3>(rng); break;
            case 4: runRandomInverseCheck<4>(rng); break;
            case 5: runRandomInverseCheck<5>(rng); break;
            case 6: runRandomInverseCheck<6>(rng); break;
        }
    }
}

TEST(matrixTest, StaticCastTest) {
    matrix<double, 2,2> M1 = {1.1, 2.2, 3.3, 4.4};
    matrix<int, 2,2> M2 = static_cast<matrix<int,2,2>>(M1);
    
    M1 = static_cast<matrix<double,2,2>>(M2);

    EXPECT_TRUE((std::is_same_v<decltype(M1), matrix<double, 2,2>>));
    EXPECT_TRUE((std::is_same_v<decltype(M2), matrix<int, 2,2>>));

    int value = 1;    
    for (std::size_t row = 0; row < 2; row++) {
        for (std::size_t column = 0; column < 2; column++) {
            EXPECT_EQ(M2(row, column), value); 
            EXPECT_DOUBLE_EQ(M1(row, column), static_cast<double>(value));
            value += 1;
        }
    }   
}