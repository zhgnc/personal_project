#ifndef TO_TRANSFORMATION_MATRIX_INL
#define TO_TRANSFORMATION_MATRIX_INL

template<typename type>
matrix<type,3,3> to_transformation_matrix(const rot_vec<type>& rotation_vector) {
    quat<type> q = to_quat(rotation_vector);
    return to_transformation_matrix(q);
};

template<typename type>
matrix<type,3,3> to_transformation_matrix(const quat<type>& quaternion) {
    return to_rotation_matrix(quaternion).transpose();
};

#endif