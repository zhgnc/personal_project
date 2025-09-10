// Copyright 2025 Zachary Hart

#include <array>
#include <iostream>

#include <math_utilities/matrix/matrix.hpp>
#include <math_utilities/quaternion/quat.hpp>
#include <math_utilities/vector/vector.hpp>
#include <math_utilities/rotation_vector/rot_vec.hpp>
#include <math_utilities/transforms/transforms.hpp>

template <typename T> T pi(3.1415926535);

int main() { 
    matrix<double, 3,3> dcm = {1,0,0, 0,1,0, 0,0,1};
    // dcm.print();

    quat<double> quat_1 = to_quat(dcm);
    quat_1.print();

    // dcm = {0,1,0, -1,0,0, 0,0,1};
    // quat = to_quat(dcm);
    // quat.print();

    // dcm = {0,0,1, 0,1,0, -1,0,0};
    // quat = to_quat(dcm);
    // quat.print();

    // dcm = {1,0,0, 0,0,1, 0,-1,0};
    // quat = to_quat(dcm);
    // quat.print();

    rot_vec<double> rot_vec_1 = {{1,2,3}, 0.5};
    rot_vec_1.print();

    quat_1 = to_quat(rot_vec_1);
    quat_1.print();

    rot_vec<double> rot_vec_2 = to_rot_vec(quat_1);
    rot_vec_2.print();

    // rot_vec_1 = {0.0, 0.0, 0.0};
    matrix<double, 3,3> dcm_2 = to_dcm(rot_vec_1);
    dcm_2.print(); 

    quat<double> quat_2 = {0,0,0,1};
    dcm_2 = to_dcm(quat_2);
    dcm_2.print();
    
    return 0; 
};
