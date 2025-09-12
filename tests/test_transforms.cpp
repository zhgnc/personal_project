#include <gtest/gtest.h>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <random>
#include "../src/math_utilities/transforms/transforms.hpp"
#include "../src/math_utilities/matrix/matrix.hpp"
#include "../src/math_utilities/rotation_vector/rot_vec.hpp"
#include "../src/math_utilities/quaternion/quat.hpp"

inline quat<double> toMyQuat(const Eigen::Quaternion<double>& q) {
    quat<double> my_quat = {q.x(), q.y(), q.z(), q.w()};
    return my_quat;
}

TEST(transformsTest, QuatToRotationAndTransformationMatrix) {
    std::mt19937 rng(0);
    std::uniform_real_distribution<double> dist(-1.0, 1.0);
    int number_of_tests = 100; 

    for (std::size_t i = 0; i < number_of_tests; i++) {
        Eigen::Quaternion<double> q_eigen(dist(rng), dist(rng), dist(rng), dist(rng));
        q_eigen.normalize();

        quat<double> q_me = toMyQuat(q_eigen);

        matrix<double, 3, 3> rot_mat_me   = to_rotation_matrix(q_me);
        matrix<double, 3, 3> trans_mat_me = to_transformation_matrix(q_me);
        Eigen::Matrix3d rot_vec_eigen     = q_eigen.toRotationMatrix();

        trans_mat_me = trans_mat_me.transpose();

        for (std::size_t row = 0; row < 3; row++) {
            for (std::size_t column = 0; column < 3; column++) {
                EXPECT_NEAR(rot_mat_me(row, column), rot_vec_eigen(row, column), 1e-9);
                EXPECT_NEAR(rot_mat_me(row, column), trans_mat_me(row, column), 1e-9);
            }
        }
    }
}

inline matrix<double, 3,3> eigenMatoMyMat(Eigen::Matrix3d dcm_in) {
    matrix<double, 3,3> output; 
    
    for (std::size_t row = 0; row < 3; row ++) {
        for (std::size_t column = 0; column < 3; column++) {
            output(row, column) = dcm_in(row, column); 
        }
    }

    return output.transpose(); 
}

TEST(transformsTest, RotationAndTransformationMatrixestoQuat) {
    std::mt19937 rng(0);
    std::uniform_real_distribution<double> angle_dist(-3.14, 3.14);
    int number_of_tests = 100; 

    for (std::size_t i = 0; i < number_of_tests; i++) { 
        double roll  = angle_dist(rng);
        double pitch = angle_dist(rng);
        double yaw   = angle_dist(rng);

        Eigen::Matrix3d rot_mat_eigen;
        rot_mat_eigen = Eigen::AngleAxisd(yaw,   Eigen::Vector3d::UnitZ()) *
                        Eigen::AngleAxisd(pitch, Eigen::Vector3d::UnitY()) *
                        Eigen::AngleAxisd(roll,  Eigen::Vector3d::UnitX());
        
        matrix<double, 3,3> trans_mat_me = eigenMatoMyMat(rot_mat_eigen); 

        Eigen::Quaternion<double> q_eigen(rot_mat_eigen);
        quat<double> q_trans_mat_me = to_quat(trans_mat_me);

        double dot_product_test = q_trans_mat_me(0)*q_eigen.x() +
                                  q_trans_mat_me(1)*q_eigen.y() +
                                  q_trans_mat_me(2)*q_eigen.z() +
                                  q_trans_mat_me(3)*q_eigen.w();
        
        if (dot_product_test < 0) {
            q_trans_mat_me = q_trans_mat_me.neg();
        }

        EXPECT_NEAR(q_trans_mat_me(0), q_eigen.x(), 1e-9);
        EXPECT_NEAR(q_trans_mat_me(1), q_eigen.y(), 1e-9);
        EXPECT_NEAR(q_trans_mat_me(2), q_eigen.z(), 1e-9);
        EXPECT_NEAR(q_trans_mat_me(3), q_eigen.w(), 1e-9);
    }
}