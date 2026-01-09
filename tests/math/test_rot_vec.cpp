#include <gtest/gtest.h>
#include "../src/math/rotation_vector/rot_vec.hpp"

TEST(rotVecTest, ConstructorAndAccessTest) {
    rot_vec<int> v1 = {1, 2, 3};
    EXPECT_EQ(v1(0), 1);
    EXPECT_EQ(v1(1), 2);
    EXPECT_EQ(v1(2), 3);

    v1(1) = 100;
    EXPECT_EQ(v1(0), 1);
    EXPECT_EQ(v1(1), 100);
    EXPECT_EQ(v1(2), 3);

    rot_vec<int> v2;
    EXPECT_EQ(v2(0), 0);
    EXPECT_EQ(v2(1), 0);
    EXPECT_EQ(v2(2), 0);

    vector<double, 3> axis = {1.0, 2.0, 3.0};
    double angle           = 10.0; 
    rot_vec<double> v3     = {axis, angle};
    
    EXPECT_NEAR(v3(0), 10.0/std::sqrt(14.0), 1e-9);
    EXPECT_NEAR(v3(1), 20.0/std::sqrt(14.0), 1e-9);
    EXPECT_NEAR(v3(2), 30.0/std::sqrt(14.0), 1e-9);
}

TEST(rotVecTest, TestGettingAxisAndAngle) {
    vector<double, 3> axis = {1.0, 2.0, 3.0};
    double angle           = 10.0; 
    rot_vec<double> v1     = {axis, angle};
    
    EXPECT_NEAR(v1.angle(), 10.0, 1e-9);
    
    vector<double,3> axis_test = v1.axis();
    EXPECT_NEAR(axis_test(0), 1.0/std::sqrt(14.0), 1e-9);
    EXPECT_NEAR(axis_test(1), 2.0/std::sqrt(14.0), 1e-9);
    EXPECT_NEAR(axis_test(2), 3.0/std::sqrt(14.0), 1e-9);
}

TEST(rotVecTest, InhertianceTestFromMatrix) {
    rot_vec<int> v1 = {1, 2, 3};
    rot_vec<int> v2 = {4, 5, 6};
    rot_vec<int> v3;

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

    rot_vec<float> v4 = {1,2,3};
    rot_vec<float> v5 = v4 / 5.0;
    EXPECT_NEAR(v5(0), 0.2, 1e-6);
    EXPECT_NEAR(v5(1), 0.4, 1e-6);
    EXPECT_NEAR(v5(2), 0.6, 1e-6);
}

TEST(rotVecTest, StaticCastTest) {
    rot_vec<double> rot_vec_1 = {1.1, 2.2, 3.3};
    rot_vec<int> rot_vec_2 = static_cast<rot_vec<int>>(rot_vec_1);
    
    rot_vec_1 = static_cast<rot_vec<double>>(rot_vec_2);

    EXPECT_TRUE((std::is_same_v<decltype(rot_vec_1), rot_vec<double>>));
    EXPECT_TRUE((std::is_same_v<decltype(rot_vec_2), rot_vec<int>>));

    int value = 1;    
    for (std::size_t row = 0; row < 3; row++) {
        EXPECT_EQ(rot_vec_2(row), value); 
        EXPECT_DOUBLE_EQ(rot_vec_1(row), static_cast<double>(value));
        value += 1;
    }   
}