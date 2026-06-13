#ifndef TO_TRANSFORMATION_MATRIX_INL
#define TO_TRANSFORMATION_MATRIX_INL

template<typename T>
matrix<T,3,3> to_transformation_matrix(const rot_vec<T>& rotation_vector) {
    quat<T> q = to_quat(rotation_vector);
    return to_transformation_matrix(q);
};

template<typename T>
matrix<T,3,3> to_transformation_matrix(const quat<T>& quaternion) {
    return to_rotation_matrix(quaternion).transpose();
};

#endif