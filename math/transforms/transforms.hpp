#ifndef TRANSFORMS_DECLARATIONS_HPP
#define TRANSFORMS_DECLARATIONS_HPP

#include "../matrix/matrix.hpp"
#include "../vector/vector.hpp"
#include "../quaternion/quat.hpp"
#include "../rotation_vector/rot_vec.hpp"

// Equations referenced in this section are from "Rotations, 
// Transformations, Left Quaternions, Right Quaternions?" by Zanetti

template<typename type>
quat<type> to_quat(const rot_vec<type>& rotation_vector);
template<typename type>
quat<type> to_quat(const matrix<type,3,3>& matrix);

template<typename type>
matrix<type,3,3> to_rotation_matrix(const rot_vec<type>& rotation_vector);
template<typename type>
matrix<type,3,3> to_rotation_matrix(const quat<type>& quaternion);

template<typename type>
matrix<type,3,3> to_transformation_matrix(const rot_vec<type>& rotation_vector);
template<typename type>
matrix<type,3,3> to_transformation_matrix(const quat<type>& quaternion);

template<typename type>
rot_vec<type> to_rot_vec(const quat<type>& quaternion);
template<typename type>
rot_vec<type> to_rot_vec(const matrix<type,3,3>& dcm);


#include "to_quat.inl"
#include "to_rotation_matrix.inl"
#include "to_transformation_matrix.inl"
#include "to_rot_vec.inl"

#endif