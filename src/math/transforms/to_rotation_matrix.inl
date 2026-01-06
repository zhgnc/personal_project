#ifndef TO_ROTATION_MATRIX_INL
#define TO_ROTATION_MATRIX_INL

template<typename T>
matrix<T,3,3> to_rotation_matrix(const rot_vec<T>& rotation_vector) {
    return to_transformation_matrix(rotation_vector).transpose();
}

template<typename T>
matrix<T,3,3> to_rotation_matrix(const quat<T>& quaternion) {
    matrix<T, 3,3> cross_product_matrix = {0.0,           -quaternion(2),  quaternion(1),
                                           quaternion(2),            0.0, -quaternion(0), 
                                          -quaternion(1),  quaternion(0),            0.0};
    matrix<T, 3,3> term_1 = 2.0 * quaternion(3) * cross_product_matrix;
    matrix<T, 3,3> term_2 = 2.0 * cross_product_matrix.pow(2);

    return identityMatrix<T,3>() + term_1 + term_2;
}

#endif