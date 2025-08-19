#include <gtest/gtest.h>
#include "../src/math_utilities/matrix2D.hpp"  

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
    Matrix2D<double, 3, 3> output      = test_matrix.set_row(0, {10, 11, 12});
    Matrix2D<double, 3, 3> expected    = {10, 11, 12, 4, 5, 6, 7, 8, 9};

    for (size_t rows = 0; rows < expected.num_rows; rows++) {
        for (size_t columns = 0; columns < expected.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected(rows, columns), output(rows, columns));
        }
    }
}

TEST(Matrix2DTest, MatrixSetColumn) {   
    Matrix2D<double, 3, 3> test_matrix = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
    Matrix2D<double, 3, 3> output      = test_matrix.set_column(1, {10, 11, 12});
    Matrix2D<double, 3, 3> expected    = {1, 10, 3, 4, 11, 6, 7, 12, 9};

    for (size_t rows = 0; rows < expected.num_rows; rows++) {
        for (size_t columns = 0; columns < expected.num_columns; columns++) {
            EXPECT_DOUBLE_EQ(expected(rows, columns), output(rows, columns));
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

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
