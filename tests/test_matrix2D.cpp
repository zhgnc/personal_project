#include <gtest/gtest.h>
#include "../src/math_utilities/matrix2D.hpp"  

// Test constructor and element access
TEST(Matrix2DTest, ElementAccess) {
    Matrix2D<int, 2, 2> mat = {1, 2, 3, 4};
    EXPECT_EQ(mat(0, 0), 1);
    EXPECT_EQ(mat(0, 1), 2);
    EXPECT_EQ(mat(1, 0), 3);
    EXPECT_EQ(mat(1, 1), 4);
}

TEST(Matrix2DTest, MatrixMultiplicationInt) {   
    Matrix2D<int, 3, 3> test_matrix_1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
    Matrix2D<int, 3, 3> test_matrix_2 = {10, 11, 12, 13, 14, 15, 16, 17, 18};
    Matrix2D<int, 3, 3> output_1      = test_matrix_1 * test_matrix_2;
    Matrix2D<int, 3, 3> expected_1    = {84, 90, 96, 201, 216, 231, 318, 342, 366};

    for (size_t rows = 0; rows < expected_1.num_rows; ++rows) {
        for (size_t columns = 0; columns < expected_1.num_columns; ++columns) {
            EXPECT_EQ(expected_1(rows, columns), output_1(rows, columns));
        }
    }

    Matrix2D<int, 3, 2> test_matrix_3 = {1, 2, 3, 4, 5, 6};  
    Matrix2D<int, 2, 3> test_matrix_4 = {10, 11, 12, 13, 14, 15};
    Matrix2D<int, 3, 3> output_2      = test_matrix_3 * test_matrix_4;
    Matrix2D<int, 3, 3> expected_2    = {36, 39, 42, 82, 89, 96, 128, 139, 150};

    for (size_t rows = 0; rows < expected_2.num_rows; ++rows) {
        for (size_t columns = 0; columns < expected_2.num_columns; ++columns) {
            EXPECT_EQ(expected_2(rows, columns), output_2(rows, columns));
        }
    }

    Matrix2D<int, 2, 2> output_3   = test_matrix_4 * test_matrix_3;
    Matrix2D<int, 2, 2> expected_3 = {103, 136, 130, 172};

    for (size_t rows = 0; rows < expected_3.num_rows; ++rows) {
        for (size_t columns = 0; columns < expected_3.num_columns; ++columns) {
            EXPECT_EQ(expected_3(rows, columns), output_3(rows, columns));
        }
    }

    Matrix2D<int, 3, 2> output_4   = test_matrix_1 * test_matrix_3;
    Matrix2D<int, 3, 2> expected_4 = {22, 28, 49, 64, 76, 100};

    for (size_t rows = 0; rows < expected_4.num_rows; ++rows) {
        for (size_t columns = 0; columns < expected_4.num_columns; ++columns) {
            EXPECT_EQ(expected_4(rows, columns), output_4(rows, columns));
        }
    }
}

TEST(Matrix2DTest, MatrixMultiplicationFloat) {   
    Matrix2D<float, 3, 3> test_matrix_1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
    Matrix2D<float, 3, 3> test_matrix_2 = {10, 11, 12, 13, 14, 15, 16, 17, 18};
    Matrix2D<float, 3, 3> output_1      = test_matrix_1 * test_matrix_2;
    Matrix2D<float, 3, 3> expected_1    = {84, 90, 96, 201, 216, 231, 318, 342, 366};

    for (size_t rows = 0; rows < expected_1.num_rows; ++rows) {
        for (size_t columns = 0; columns < expected_1.num_columns; ++columns) {
            EXPECT_EQ(expected_1(rows, columns), output_1(rows, columns));
        }
    }

    Matrix2D<float, 3, 2> test_matrix_3 = {1, 2, 3, 4, 5, 6};  
    Matrix2D<float, 2, 3> test_matrix_4 = {10, 11, 12, 13, 14, 15};
    Matrix2D<float, 3, 3> output_2      = test_matrix_3 * test_matrix_4;
    Matrix2D<float, 3, 3> expected_2    = {36, 39, 42, 82, 89, 96, 128, 139, 150};

    for (size_t rows = 0; rows < expected_2.num_rows; ++rows) {
        for (size_t columns = 0; columns < expected_2.num_columns; ++columns) {
            EXPECT_EQ(expected_2(rows, columns), output_2(rows, columns));
        }
    }

    Matrix2D<float, 2, 2> output_3   = test_matrix_4 * test_matrix_3;
    Matrix2D<float, 2, 2> expected_3 = {103, 136, 130, 172};

    for (size_t rows = 0; rows < expected_3.num_rows; ++rows) {
        for (size_t columns = 0; columns < expected_3.num_columns; ++columns) {
            EXPECT_EQ(expected_3(rows, columns), output_3(rows, columns));
        }
    }

    Matrix2D<float, 3, 2> output_4   = test_matrix_1 * test_matrix_3;
    Matrix2D<float, 3, 2> expected_4 = {22, 28, 49, 64, 76, 100};

    for (size_t rows = 0; rows < expected_4.num_rows; ++rows) {
        for (size_t columns = 0; columns < expected_4.num_columns; ++columns) {
            EXPECT_EQ(expected_4(rows, columns), output_4(rows, columns));
        }
    }
}

TEST(Matrix2DTest, MatrixMultiplicationDouble) {   
    Matrix2D<double, 3, 3> test_matrix_1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};  
    Matrix2D<double, 3, 3> test_matrix_2 = {10, 11, 12, 13, 14, 15, 16, 17, 18};
    Matrix2D<double, 3, 3> output_1      = test_matrix_1 * test_matrix_2;
    Matrix2D<double, 3, 3> expected_1    = {84, 90, 96, 201, 216, 231, 318, 342, 366};

    for (size_t rows = 0; rows < expected_1.num_rows; ++rows) {
        for (size_t columns = 0; columns < expected_1.num_columns; ++columns) {
            EXPECT_EQ(expected_1(rows, columns), output_1(rows, columns));
        }
    }

    Matrix2D<double, 3, 2> test_matrix_3 = {1, 2, 3, 4, 5, 6};  
    Matrix2D<double, 2, 3> test_matrix_4 = {10, 11, 12, 13, 14, 15};
    Matrix2D<double, 3, 3> output_2      = test_matrix_3 * test_matrix_4;
    Matrix2D<double, 3, 3> expected_2    = {36, 39, 42, 82, 89, 96, 128, 139, 150};

    for (size_t rows = 0; rows < expected_2.num_rows; ++rows) {
        for (size_t columns = 0; columns < expected_2.num_columns; ++columns) {
            EXPECT_EQ(expected_2(rows, columns), output_2(rows, columns));
        }
    }

    Matrix2D<double, 2, 2> output_3   = test_matrix_4 * test_matrix_3;
    Matrix2D<double, 2, 2> expected_3 = {103, 136, 130, 172};

    for (size_t rows = 0; rows < expected_3.num_rows; ++rows) {
        for (size_t columns = 0; columns < expected_3.num_columns; ++columns) {
            EXPECT_EQ(expected_3(rows, columns), output_3(rows, columns));
        }
    }

    Matrix2D<double, 3, 2> output_4   = test_matrix_1 * test_matrix_3;
    Matrix2D<double, 3, 2> expected_4 = {22, 28, 49, 64, 76, 100};

    for (size_t rows = 0; rows < expected_4.num_rows; ++rows) {
        for (size_t columns = 0; columns < expected_4.num_columns; ++columns) {
            EXPECT_EQ(expected_4(rows, columns), output_4(rows, columns));
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
