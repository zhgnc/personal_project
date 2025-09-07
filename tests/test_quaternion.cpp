#include <gtest/gtest.h>
#include <Eigen/Geometry>
#include <random>
#include <cmath>
#include "../src/math_utilities/quaternion/quat.hpp"

quat<double> toMyQuat(const Eigen::Quaternion<double>& q) {
    quat<double> my_quat = {q.x(), q.y(), q.z(), q.w()};
    return my_quat;
}

void printEigenQuat(const Eigen::Quaternion<double>& q) {
    std::cout << q.w() << "\n"
              << q.x() << "\n"
              << q.y() << "\n"
              << q.z() << "\n\n";
}

TEST(quatTest, QuatMultiplyVsEigen) {
    std::mt19937 rng(0); 
    std::uniform_real_distribution<double> dist(-1.0, 1.0);

    for (int i = 0; i < 100; i++) {
        Eigen::Quaternion<double> q_1_to_2_eigen(dist(rng), dist(rng), dist(rng), dist(rng));
        Eigen::Quaternion<double> q_2_to_3_eigen(dist(rng), dist(rng), dist(rng), dist(rng));
        q_1_to_2_eigen.normalize();
        q_2_to_3_eigen.normalize();

        // printEigenQuat(q_1_to_2_eigen);
        // printEigenQuat(q_2_to_3_eigen);

        quat<double> q_1_to_2_me = toMyQuat(q_1_to_2_eigen);
        quat<double> q_2_to_3_me = toMyQuat(q_2_to_3_eigen);

        
        // q_1_to_2_me.print();
        // q_2_to_3_me.print();

        quat<double> q_1_to_3_me                 = q_2_to_3_me    * q_1_to_2_me; // Shuster convention
        Eigen::Quaternion<double> q_1_to_3_eigen = q_2_to_3_eigen * q_1_to_2_eigen; // Hamilton convention

        // std::cout << "Eigen output: \n";
        // printEigenQuat(q_1_to_3_eigen);
        // std::cout << "My output: \n";
        // q_1_to_3_me.print();
        
        quat<double> eigen_test_quat = toMyQuat(q_1_to_3_eigen);

        for (std::size_t row = 0; row < 4; row++) {
            EXPECT_NEAR(q_1_to_3_me(row), eigen_test_quat(row), 1e-9);
        }
    }
}