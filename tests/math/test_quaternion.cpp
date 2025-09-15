#include <gtest/gtest.h>
#include <Eigen/Geometry>
#include <random>
#include <cmath>
#include "../src/math_utilities/quaternion/quat.hpp"

TEST(quatTest, QuatConstructor) {
    quat<int> q1;
    EXPECT_EQ(q1(0), 0);
    EXPECT_EQ(q1(1), 0);
    EXPECT_EQ(q1(2), 0);
    EXPECT_EQ(q1(3), 1);

    quat<double> q2 = {1, 2, 3, 4};
    EXPECT_EQ(q2(0), 1);
    EXPECT_EQ(q2(1), 2);
    EXPECT_EQ(q2(2), 3);
    EXPECT_EQ(q2(3), 4);
}

TEST(quatTest, QuatUtilities) {
    quat<double> q          = {1, 2, 3, 4};
    quat<double> q_expected = {1/std::sqrt(30.0), 
                               2/std::sqrt(30.0), 
                               3/std::sqrt(30.0), 
                               4/std::sqrt(30.0)};
    
    q = q.normalize();
    EXPECT_DOUBLE_EQ(q(0), q_expected(0));
    EXPECT_DOUBLE_EQ(q(1), q_expected(1));
    EXPECT_DOUBLE_EQ(q(2), q_expected(2));
    EXPECT_DOUBLE_EQ(q(3), q_expected(3));
    
    double scalar = q.scalar();
    EXPECT_DOUBLE_EQ(scalar, q_expected(3));

    vector<double, 3> vec = q.vec();
    EXPECT_DOUBLE_EQ(vec(0), q_expected(0));
    EXPECT_DOUBLE_EQ(vec(1), q_expected(1));
    EXPECT_DOUBLE_EQ(vec(2), q_expected(2));

    q.setScalar(10.0);
    EXPECT_DOUBLE_EQ(q(3), 10.0);

    q.setVector({3.0, 4.0, 5.0});
    EXPECT_DOUBLE_EQ(q(0), 3.0);
    EXPECT_DOUBLE_EQ(q(1), 4.0);
    EXPECT_DOUBLE_EQ(q(2), 5.0);
    
    q.setIdentity();
    EXPECT_DOUBLE_EQ(q(0), 0);
    EXPECT_DOUBLE_EQ(q(1), 0);
    EXPECT_DOUBLE_EQ(q(2), 0);
    EXPECT_DOUBLE_EQ(q(3), 1);
}

TEST(quatTest, QuatInverse) {
    quat<double> q1 = {1,2,3,4};
    q1 = q1.normalize();

    quat<double> q2 = q1.inv() * q1;
    EXPECT_DOUBLE_EQ(q2(0), 0);
    EXPECT_DOUBLE_EQ(q2(1), 0);
    EXPECT_DOUBLE_EQ(q2(2), 0);
    EXPECT_DOUBLE_EQ(q2(3), 1);
}


inline quat<double> toMyQuat(const Eigen::Quaternion<double>& q) {
    quat<double> my_quat = {q.x(), q.y(), q.z(), q.w()};
    return my_quat;
}

TEST(quatTest, QuatMultiplyVsEigen) {
    std::mt19937 rng(0); 
    std::uniform_real_distribution<double> dist(-1.0, 1.0);

    for (int i = 0; i < 100; i++) {
        Eigen::Quaternion<double> q_1_to_2_eigen(dist(rng), dist(rng), dist(rng), dist(rng));
        Eigen::Quaternion<double> q_2_to_3_eigen(dist(rng), dist(rng), dist(rng), dist(rng));
        q_1_to_2_eigen.normalize();
        q_2_to_3_eigen.normalize();

        quat<double> q_1_to_2_me = toMyQuat(q_1_to_2_eigen);
        quat<double> q_2_to_3_me = toMyQuat(q_2_to_3_eigen);

        quat<double> q_1_to_3_me                 = q_2_to_3_me    * q_1_to_2_me; // Shuster convention
        Eigen::Quaternion<double> q_1_to_3_eigen = q_2_to_3_eigen * q_1_to_2_eigen; // Hamilton convention

        quat<double> eigen_test_quat = toMyQuat(q_1_to_3_eigen);

        for (std::size_t row = 0; row < 4; row++) {
            EXPECT_NEAR(q_1_to_3_me(row), eigen_test_quat(row), 1e-9);
        }
    }
}

TEST(quatTest, RotateVectorUsingQuatVsEigen) {
    std::mt19937 rng(0); 
    std::uniform_real_distribution<double> dist(-1.0, 1.0);

    for (int i = 0; i < 100; i++) {
        Eigen::Quaternion<double> q_eigen(dist(rng), dist(rng), dist(rng), dist(rng));
        q_eigen.normalize();

        quat<double> q_me = toMyQuat(q_eigen);

        Eigen::Vector3d v_eigen(dist(rng), dist(rng), dist(rng));
        vector<double, 3> v_me = {v_eigen(0), v_eigen(1), v_eigen(2)};

        Eigen::Vector3d v_rot_eigen = q_eigen * v_eigen;
        vector<double, 3> v_rot_me  = q_me    * v_me;

        for (std::size_t row = 0; row < 3; row++) {
            EXPECT_NEAR(v_rot_me(row), v_rot_eigen(row), 1e-9);
        }
    }
}

TEST(quatTest, StaticCastTest) {
    quat<double> q_1 = {1.0, 2.0, 3.0, 4.0};
    q_1 = q_1.normalize();

    quat<int> q_2 = static_cast<quat<int>>(q_1);
    q_1           = static_cast<quat<double>>(q_2);

    EXPECT_TRUE((std::is_same_v<decltype(q_1), quat<double>>));
    EXPECT_TRUE((std::is_same_v<decltype(q_2), quat<int>>));

    for (std::size_t row = 0; row < 4; row++) {
        EXPECT_EQ(q_2(row), 0); 
        EXPECT_DOUBLE_EQ(q_1(row), 0.0);
    }   
}