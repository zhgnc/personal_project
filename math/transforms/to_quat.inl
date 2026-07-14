#ifndef TO_QUAT_INL
#define TO_QUAT_INL

#include "transforms.hpp"

template<typename type>
quat<type> to_quat(const rot_vec<type>& rotation_vector) {
    vector<type,3> unit_axis = rotation_vector.axis(); 
    type angle               = rotation_vector.angle();
    
    quat<type> output;
    output.setScalar(std::cos(angle/2.0));
    output.setVector(unit_axis * std::sin(angle/2.0));

    return output.normalize();
};

// Matrix can be either a rotation or transformation matrix
// but the quaternion will be active
template<typename type>
quat<type> to_quat(const matrix<type,3,3>& matrix) {
    quat<type> output;

    // Shepperd's method: each radicand below equals (2*component)^2 for one quaternion
    // component, so the largest radicand identifies the largest component. Extracting
    // via the largest component keeps the 1/(4*component) divisor >= 0.5, so rounding
    // noise is never amplified. Branching on the scalar component alone (the previous
    // approach) amplified noise for rotations near 180 degrees, where the scalar is
    // tiny but still above any epsilon threshold.
    type radicand_w = 1.0 + matrix(0,0) + matrix(1,1) + matrix(2,2);
    type radicand_x = 1.0 + matrix(0,0) - matrix(1,1) - matrix(2,2);
    type radicand_y = 1.0 - matrix(0,0) + matrix(1,1) - matrix(2,2);
    type radicand_z = 1.0 - matrix(0,0) - matrix(1,1) + matrix(2,2);

    int largest_index      = 0;
    type largest_radicand  = radicand_w;

    if (radicand_x > largest_radicand) { largest_index = 1; largest_radicand = radicand_x; }
    if (radicand_y > largest_radicand) { largest_index = 2; largest_radicand = radicand_y; }
    if (radicand_z > largest_radicand) { largest_index = 3; largest_radicand = radicand_z; }

    type constant = 0;

    switch (largest_index) {
        case 0: // Scalar component is largest
            output(3)  = 0.5 * std::sqrt(largest_radicand);

            constant = 1.0 / (4.0*output(3));
            output(0)  = constant * (matrix(2,1) - matrix(1,2));
            output(1)  = constant * (matrix(0,2) - matrix(2,0));
            output(2)  = constant * (matrix(1,0) - matrix(0,1));
            break;

        case 1: // x component is largest
            output(0)  = 0.5 * std::sqrt(largest_radicand);

            constant = 1.0 / (4.0*output(0));
            output(1)  = constant * (matrix(0,1) + matrix(1,0));
            output(2)  = constant * (matrix(0,2) + matrix(2,0));
            output(3)  = constant * (matrix(2,1) - matrix(1,2));
            break;

        case 2: // y component is largest
            output(1)  = 0.5 * std::sqrt(largest_radicand);

            constant = 1.0 / (4.0*output(1));
            output(0)  = constant * (matrix(1,0) + matrix(0,1));
            output(2)  = constant * (matrix(1,2) + matrix(2,1));
            output(3)  = constant * (matrix(0,2) - matrix(2,0));
            break;

        case 3: // z component is largest
            output(2)  = 0.5 * std::sqrt(largest_radicand);

            constant = 1.0 / (4.0*output(2));
            output(0)  = constant * (matrix(2,0) + matrix(0,2));
            output(1)  = constant * (matrix(2,1) + matrix(1,2));
            output(3)  = constant * (matrix(1,0) - matrix(0,1));
            break;
    }

    // Canonicalize to the w >= 0 hemisphere so the returned sign convention is
    // deterministic and identical across all branches (+q and -q are the same rotation)
    if (output(3) < 0.0) {
        for (std::size_t i = 0; i < 4; i++) {
            output(i) = -output(i);
        }
    }

    return output.normalize();
};

#endif