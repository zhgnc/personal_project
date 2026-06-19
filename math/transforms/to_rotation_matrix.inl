#ifndef TO_ROTATION_MATRIX_INL
#define TO_ROTATION_MATRIX_INL

template<typename type>
matrix<type,3,3> to_rotation_matrix(const rot_vec<type>& rotation_vector) {
    return to_transformation_matrix(rotation_vector).transpose();
}

template<typename type>
matrix<type,3,3> to_rotation_matrix(const quat<type>& quaternion) {
    matrix<type, 3,3> cross_product_matrix = {0.0,           -quaternion(2),  quaternion(1),
                                           quaternion(2),            0.0, -quaternion(0), 
                                          -quaternion(1),  quaternion(0),            0.0};
    matrix<type, 3,3> term_1 = 2.0 * quaternion(3) * cross_product_matrix;
    matrix<type, 3,3> term_2 = 2.0 * cross_product_matrix.pow(2);

    return identityMatrix<type,3>() + term_1 + term_2;
}

#endif