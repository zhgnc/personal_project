#ifndef TRANSFORMS_DECLARATIONS_HPP
#define TRANSFORMS_DECLARATIONS_HPP

#include "../matrix/matrix.hpp"
#include "../vector/vector.hpp"
#include "../quaternion/quat.hpp"
#include "../rotation_vector/rot_vec.hpp"

template<typename T>
class transforms {
  public: 
    
    quat<T> to_quat(const rot_vec<T>& rotation_vector);
    quat<T> to_quat(const matrix<T,3,3>& dcm);

    matrix<T,3,3> to_dcm(const rot_vec<T>& rotation_vector);
    matrix<T,3,3> to_dcm(const quat<T>& quaternion);

    rot_vec<T> to_rot_vec(const quat<T>& quaternion);
    rot_vec<T> to_rot_vec(const matrix<T,3,3>& dcm);
};

#endif