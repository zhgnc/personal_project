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

// Test matrix multiplication: 2x3 * 3x2 -> 2x2
TEST(Matrix2DTest, MatrixMultiplication) {
    Matrix2D<int, 2, 3> mat1 = {1, 2, 3, 4, 5, 6};
    Matrix2D<int, 3, 2> mat2 = {7, 8, 9, 10, 11, 12};

    Matrix2D<int, 2, 2> expected = {58, 64, 139, 154};
    auto result = mat1 * mat2;

    for (size_t r = 0; r < 2; ++r) {
        for (size_t c = 0; c < 2; ++c) {
            EXPECT_EQ(result(r, c), expected(r, c));
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
