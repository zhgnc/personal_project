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

    v3 = 2 * v2 * 3;
    EXPECT_EQ(v3(0), 24);
    EXPECT_EQ(v3(1), 30);
    EXPECT_EQ(v3(2), 36);

    v3 = v1 + v2;
    EXPECT_EQ(v3(0), 5);
    EXPECT_EQ(v3(1), 7);
    EXPECT_EQ(v3(2), 9);

    v3 = 10 + v1 + 25;
    EXPECT_EQ(v3(0), 36);
    EXPECT_EQ(v3(1), 37);
    EXPECT_EQ(v3(2), 38);

    v3 = v1 - (2*v2);
    EXPECT_EQ(v3(0), -7);
    EXPECT_EQ(v3(1), -8);
    EXPECT_EQ(v3(2), -9);

    v3 = v1 - 25;
    EXPECT_EQ(v3(0), -24);
    EXPECT_EQ(v3(1), -23);
    EXPECT_EQ(v3(2), -22);

    vector<float, 3> v4 = {1,2,3};
    vector<float, 3> v5;
    v5 = v4 / 5.0;
    EXPECT_NEAR(v5(0), 0.2, 1e-6);
    EXPECT_NEAR(v5(1), 0.4, 1e-6);
    EXPECT_NEAR(v5(2), 0.6, 1e-6);

    matrix<int, 1,3> m1 = v1.transpose();
    EXPECT_EQ(m1(0,0), 1);
    EXPECT_EQ(m1(0,1), 2);
    EXPECT_EQ(m1(0,2), 3);
}

TEST(vectorTest, MatrixTimesVector) {
    matrix<int, 3,3> m1 = {1,2,3, 4,5,6, 7,8,9};
    vector<int, 3>   v1 = {3,5,7};
    vector<int, 3>   v2 = m1 * v1;

    EXPECT_EQ(v2(0), 34);
    EXPECT_EQ(v2(1), 79);
    EXPECT_EQ(v2(2), 124);
}

TEST(vectorTest, DotProduct) {
    vector<int, 3>   v1 = {1,2,3};
    vector<int, 3>   v2 = {4,5,6};
    int output = v1.dot(v2);

    EXPECT_EQ(output, 32);
}

TEST(vectorTest, CrossProduct) {
    vector<int, 3> v1 = {4,5,6};
    vector<int, 3> v2 = {4,120,2000};
    vector<int, 3> v3 = v1.cross(v2);

    EXPECT_EQ(v3(0),  9280);
    EXPECT_EQ(v3(1), -7976);
    EXPECT_EQ(v3(2),   460);
}

TEST(vectorTest, AngleBetweenVectors) {
    vector<double, 2> v1;
    vector<double, 2> v2 = {1, 0};
    double angle  = 0.0;
    double output = 0.0; 
    double pi      = 3.14159265359;
    double rad2deg = 180.0 / pi;
    double deg2rad = 1.0 / rad2deg;

    for (std::size_t i = 0; i < 360; i++) {
        angle  = i * deg2rad;
        v1     = {std::cos(angle), std::sin(angle)};
        output = v1.angle_between(v2);

        if (i > 180) {
            angle = 2*pi - angle;
        }

        EXPECT_NEAR(angle, output, 1e-6);
    }
}

TEST(vectorTest, StaticCastTest) {
    vector<double,2> v1 = {1.1, 2.2};
    vector<int, 2> v2   = static_cast<vector<int,2>>(v1);
    
    v1 = static_cast<vector<double,2>>(v2);

    EXPECT_TRUE((std::is_same_v<decltype(v1), vector<double,2>>));
    EXPECT_TRUE((std::is_same_v<decltype(v2), vector<int, 2>>));

    int value = 1;    
    for (std::size_t row = 0; row < 2; row++) {
        EXPECT_EQ(v2(row), value); 
        EXPECT_DOUBLE_EQ(v1(row), static_cast<double>(value));
        value += 1;
    }   
}