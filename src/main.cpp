// Copyright 2025 Zachary Hart

#include <array>
#include <iostream>

#include <math_utilities/matrix/matrix.hpp>
#include <math_utilities/quaternion/quat.hpp>
#include <math_utilities/rotation_vector/rot_vec.hpp>
#include <math_utilities/transforms/transforms.hpp>
#include <math_utilities/vector/vector.hpp>

template <typename T> T pi(3.1415926535);

int main() {
  // matrix<double, 3, 3> dcm = {1, 0, 0, 0, 1, 0, 0, 0, 1};
  // // dcm.print();

  // quat<double> quat_1 = to_quat(dcm);
  // quat_1.print();

  // // dcm = {0,1,0, -1,0,0, 0,0,1};
  // // quat = to_quat(dcm);
  // // quat.print();

  // // dcm = {0,0,1, 0,1,0, -1,0,0};
  // // quat = to_quat(dcm);
  // // quat.print();

  // // dcm = {1,0,0, 0,0,1, 0,-1,0};
  // // quat = to_quat(dcm);
  // // quat.print();

  // rot_vec<double> rot_vec_1 = {{1, 2, 3}, 0.5};
  // rot_vec_1.print();

  // quat_1 = to_quat(rot_vec_1);
  // quat_1.print();

  // rot_vec<double> rot_vec_2 = to_rot_vec(quat_1);
  // rot_vec_2.print();

  // // rot_vec_1 = {0.0, 0.0, 0.0};
  // matrix<double, 3, 3> dcm_2 = to_dcm(rot_vec_1);
  // dcm_2.print();

  // quat<double> quat_2 = {0, 0, 0, 1};
  // dcm_2 = to_dcm(quat_2);
  // dcm_2.print();

  matrix<double, 3, 3> dcm = {0, 1, 0, -1, 0, 0, 0, 0, 1};
  vector<double, 3> vec = {1, 0, 0};
  vector<double, 3> vec_1 = dcm * vec;
  vector<double, 3> vec_2 = dcm.transpose() * vec;
  dcm.print();
  vec_1.print();
  vec_2.print();

  quat<double> q = to_quat(dcm);
  q.print();

  vector<double, 3> vec_3 = q * vec;
  matrix<double, 3, 3> rot_mat = to_rotation_matrix(q);
  matrix<double, 3, 3> trans_mat = to_transformation_matrix(q);

  // rot_mat   = static_cast<matrix<int,3,3>>(rot_mat);
  // trans_mat = static_cast<matrix<int,3,3>>(trans_mat);

  vec_3.print();
  rot_mat.print();
  trans_mat.print();

  vector<double, 3> vec_4 = rot_mat * vec;
  vector<double, 3> vec_5 = trans_mat * vec;
  vec_4.print();
  vec_5.print();

  return 0;
};
