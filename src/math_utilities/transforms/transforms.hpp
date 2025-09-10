#ifndef TRANSFORMS_DECLARATIONS_HPP
#define TRANSFORMS_DECLARATIONS_HPP

#include "../matrix/matrix.hpp"
#include "../vector/vector.hpp"
#include "../quaternion/quat.hpp"
#include "../rotation_vector/rot_vec.hpp"

template<typename T>
quat<T> to_quat(const rot_vec<T>& rotation_vector);
template<typename T>
quat<T> to_quat(const matrix<T,3,3>& dcm);

template<typename T>
matrix<T,3,3> to_dcm(const rot_vec<T>& rotation_vector);
template<typename T>
matrix<T,3,3> to_dcm(const quat<T>& quaternion);

template<typename T>
rot_vec<T> to_rot_vec(const quat<T>& quaternion);
template<typename T>
rot_vec<T> to_rot_vec(const matrix<T,3,3>& dcm);


#include "to_quat.inl"
#include "to_dcm.inl"
#include "to_rot_vec.inl"

#endif