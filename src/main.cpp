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
    dcm.print();

    transforms<double> tf;
    quat<double> quat = tf.to_quat(dcm);
    quat.print();

    dcm = {0,1,0, -1,0,0, 0,0,1};
    quat = tf.to_quat(dcm);
    quat.print();

    dcm = {0,0,1, 0,1,0, -1,0,0};
    quat = tf.to_quat(dcm);
    quat.print();

    dcm = {1,0,0, 0,0,1, 0,-1,0};
    quat = tf.to_quat(dcm);
    quat.print();
    
    return 0; 
};
