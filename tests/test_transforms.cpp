#include <gtest/gtest.h>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <random>
#include "../src/math_utilities/transforms/transforms.hpp"
#include "../src/math_utilities/matrix/matrix.hpp"
#include "../src/math_utilities/rotation_vector/rot_vec.hpp"
#include "../src/math_utilities/quaternion/quat.hpp"

TEST(transformsTest, SimpleRotationTest) {
  matrix<double,3,3> dcm_1_to_2 = {0,1,0, -1,0,0, 0,0,1};
  vector<double,3>   vec        = {1,0,0};
  vector<double,3>   vec_2      = dcm_1_to_2 * vec;
  vector<double,3>   vec_1      = dcm_1_to_2.transpose() * vec;

  EXPECT_NEAR(vec_2(0),  0, 1e-9);
  EXPECT_NEAR(vec_2(1), -1, 1e-9);
  EXPECT_NEAR(vec_2(2),  0, 1e-9);

  EXPECT_NEAR(vec_1(0), 0, 1e-9);
  EXPECT_NEAR(vec_1(1), 1, 1e-9);
  EXPECT_NEAR(vec_1(2), 0, 1e-9);


  quat<double> q_1_to_2 = to_quat(dcm_1_to_2);

  vector<double,3> vec_from_active_quat_rot = q_1_to_2 * vec;
  EXPECT_NEAR(vec_from_active_quat_rot(0),  0, 1e-9);
  EXPECT_NEAR(vec_from_active_quat_rot(1), -1, 1e-9);
  EXPECT_NEAR(vec_from_active_quat_rot(2),  0, 1e-9);

  matrix<double,3,3> rot_mat_1_to_2   = to_rotation_matrix(q_1_to_2);
  matrix<double,3,3> trans_mat_2_to_1 = to_transformation_matrix(q_1_to_2);

  vector<double,3> vec_from_rot_mat   = rot_mat_1_to_2 * vec;
  vector<double,3> vec_from_trans_mat = trans_mat_2_to_1 * vec;


  EXPECT_NEAR(vec_2(0), vec_from_rot_mat(0), 1e-9);
  EXPECT_NEAR(vec_2(1), vec_from_rot_mat(1), 1e-9);
  EXPECT_NEAR(vec_2(2), vec_from_rot_mat(2), 1e-9);

  EXPECT_NEAR(vec_1(0), vec_from_trans_mat(0), 1e-9);
  EXPECT_NEAR(vec_1(1), vec_from_trans_mat(1), 1e-9);
  EXPECT_NEAR(vec_1(2), vec_from_trans_mat(2), 1e-9);
}


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

    return output; 
}

TEST(transformsTest, RotationMatrixtoQuat) {
    std::mt19937 rng(0);
    std::uniform_real_distribution<double> angle_dist(-3.14, 3.14);
    int number_of_tests = 1; 

    for (std::size_t i = 0; i < number_of_tests; i++) { 
        double roll  = angle_dist(rng);
        double pitch = angle_dist(rng);
        double yaw   = angle_dist(rng);

        Eigen::Matrix3d rot_mat_eigen;
        rot_mat_eigen = Eigen::AngleAxisd(yaw,   Eigen::Vector3d::UnitZ()) *
                        Eigen::AngleAxisd(pitch, Eigen::Vector3d::UnitY()) *
                        Eigen::AngleAxisd(roll,  Eigen::Vector3d::UnitX());
        
        matrix<double, 3,3> rot_mat_me = eigenMatoMyMat(rot_mat_eigen); 

        Eigen::Quaternion<double> q_eigen(rot_mat_eigen);
        quat<double> q_rot_mat_me = to_quat(rot_mat_me);

        double dot_product_test = q_rot_mat_me(0)*q_eigen.x() +
                                  q_rot_mat_me(1)*q_eigen.y() +
                                  q_rot_mat_me(2)*q_eigen.z() +
                                  q_rot_mat_me(3)*q_eigen.w();
        
        if (dot_product_test < 0) {
            q_rot_mat_me = q_rot_mat_me.neg();
        }

        EXPECT_NEAR(q_rot_mat_me(0), q_eigen.x(), 1e-9);
        EXPECT_NEAR(q_rot_mat_me(1), q_eigen.y(), 1e-9);
        EXPECT_NEAR(q_rot_mat_me(2), q_eigen.z(), 1e-9);
        EXPECT_NEAR(q_rot_mat_me(3), q_eigen.w(), 1e-9);
    }
}

rot_vec<double> eigenRotVecToMyRotVec(Eigen::AngleAxisd rot_vec_in) {
    rot_vec<double> output;
    Eigen::Vector3d rot_vec_eigen = rot_vec_in.angle() * rot_vec_in.axis();

    for (std::size_t i = 0; i < 3; i++) {
        output(i) = rot_vec_eigen(i);
    }

    return output;
}

TEST(transformsTest, RotationVectorToQuaternion) {
    std::mt19937 rng(0);
    std::uniform_real_distribution<double> random_axis(-1.0, 1.0);
    std::uniform_real_distribution<double> random_angle(-3.14, 3.14);
    int number_of_tests = 100;

    for (std::size_t i = 0; i < number_of_tests; i++) {
        Eigen::Vector3d axis(random_axis(rng), random_axis(rng), random_axis(rng));
        double angle = random_angle(rng);
        axis.normalize();
        Eigen::AngleAxisd rot_vec_eigen(angle, axis);
        Eigen::Quaterniond q_eigen(rot_vec_eigen);

        rot_vec<double> rot_vec_me = eigenRotVecToMyRotVec(rot_vec_eigen);
        quat<double> q_me          = to_quat(rot_vec_me);

        double dot_product_test = q_me(0)*q_eigen.x() +
                                  q_me(1)*q_eigen.y() +
                                  q_me(2)*q_eigen.z() +
                                  q_me(3)*q_eigen.w();
        
        if (dot_product_test < 0) {
            q_me = q_me.neg();
        }

        EXPECT_NEAR(q_me(0), q_eigen.x(), 1e-9);
        EXPECT_NEAR(q_me(1), q_eigen.y(), 1e-9);
        EXPECT_NEAR(q_me(2), q_eigen.z(), 1e-9);
        EXPECT_NEAR(q_me(3), q_eigen.w(), 1e-9);
    }
}

TEST(transformsTest, QuaternionToRotationMatrix) {
    std::mt19937 rng(0);
    std::uniform_real_distribution<double> dist(-1.0, 1.0);
    int number_of_tests = 1;

    for (std::size_t i = 0; i < number_of_tests; i++) {
        Eigen::Quaterniond q_eigen(dist(rng), dist(rng), dist(rng), dist(rng));
        q_eigen.normalize();
        Eigen::AngleAxisd rot_vec_eigen(q_eigen);
        Eigen::Vector3d rot_vec_ref = rot_vec_eigen.angle() * rot_vec_eigen.axis();

        quat<double> q_me = toMyQuat(q_eigen);
        rot_vec<double> rot_vec_me = to_rot_vec(q_me);

        for (std::size_t j = 0; j < 3; j++) {
            EXPECT_NEAR(rot_vec_me(j), rot_vec_ref(j), 1e-9);
        }
    }
}