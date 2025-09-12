#ifndef TO_TRANSFORMATION_MATRIX_INL
#define TO_TRANSFORMATION_MATRIX_INL

// Equation 11
template<typename T>
matrix<T,3,3> to_transformation_matrix(const rot_vec<T>& rotation_vector) {
    T angle          = rotation_vector.angle(); 
    vector<T,3> axis = rotation_vector.axis();

    matrix<T, 3,3> cross_product_matrix = {0.0,     -axis(2),  axis(1),
                                           axis(2),      0.0, -axis(0), 
                                          -axis(1),  axis(0),      0.0};
    
    matrix<T,3,3> term_1 = std::sin(angle) * cross_product_matrix;
    matrix<T,3,3> term_2 = (1.0 - std::cos(angle)) * cross_product_matrix.pow(2);

    return identityMatrix<T,3>() - term_1 + term_2;
};

// Equation 9
template<typename T>
matrix<T,3,3> to_transformation_matrix(const quat<T>& quaternion) {
    return to_rotation_matrix(quaternion).transpose();
};

#endif