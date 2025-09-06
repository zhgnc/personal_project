#include <gtest/gtest.h>
#include "../src/math_utilities/vector/vector.hpp"

TEST(vectorTest, ConstructorAndAccessTest) {
    vector<int, 3> v1 = {1, 2, 3};
    EXPECT_EQ(v1(0), 1);
    EXPECT_EQ(v1(1), 2);
    EXPECT_EQ(v1(2), 3);

    v1(1) = 100;
    EXPECT_EQ(v1(0), 1);
    EXPECT_EQ(v1(1), 100);
    EXPECT_EQ(v1(2), 3);

    vector<int, 3> v2;
    EXPECT_EQ(v2(0), 0);
    EXPECT_EQ(v2(1), 0);
    EXPECT_EQ(v2(2), 0);
}

TEST(vectorTest, InhertianceTestFromMatrix) {
    vector<int, 3> v1 = {1, 2, 3};
    vector<int, 3> v2 = {4, 5, 6};
    vector<int, 3> v3;

    EXPECT_DEATH(v3 = v1 * v2, "");

    v3 = 2 * v2 * 3;
    EXPECT_EQ(v3(0), 24);
    EXPECT_EQ(v3(1), 30);
    EXPECT_EQ(v3(2), 36);

    EXPECT_DEATH(v1 *= v2, "");

    v3 = v1 + v2;
    EXPECT_EQ(v3(0), 5);
    EXPECT_EQ(v3(1), 7);
    EXPECT_EQ(v3(2), 9);

    v3 = 10 + v1 + 25;
    EXPECT_EQ(v3(0), 36);
    EXPECT_EQ(v3(1), 37);
    EXPECT_EQ(v3(2), 38);

    v3 = v1 - (2*v2);
    EXPECT_EQ(v3(0), 7);
    EXPECT_EQ(v3(1), 8);
    EXPECT_EQ(v3(2), 9);

    v3 = v1 - 25;
    EXPECT_EQ(v3(0), -24);
    EXPECT_EQ(v3(1), -23);
    EXPECT_EQ(v3(2), -22);

    vector<float, 3> v4 = {1,2,3};
    vector<float, 3> v5;
    v5 = v4 / 5.0;
    EXPECT_DOUBLE_EQ(v5(0), 0.2);
    EXPECT_DOUBLE_EQ(v5(1), 0.4);
    EXPECT_DOUBLE_EQ(v5(2), 0.6);

    matrix<int, 1,3> m1 = v1.transpose();
    EXPECT_EQ(m1(0,0), 1);
    EXPECT_EQ(m1(0,1), 2);
    EXPECT_EQ(m1(0,2), 3);

    EXPECT_DEATH(v1.det(), "");
    EXPECT_DEATH(v1.inv(), "");
    EXPECT_DEATH(v1.pow(3), "");

    EXPECT_EQ(1,2);
}