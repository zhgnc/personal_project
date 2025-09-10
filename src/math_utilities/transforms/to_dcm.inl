#ifndef TO_DCM_INL
#define TO_DCM_INL

template<typename T>
matrix<T,3,3> to_dcm(const rot_vec<T>& rotation_vector) {
    T angle          = rotation_vector.angle(); 
    vector<T,3> axis = rotation_vector.axis();

    matrix<T,3,3> term_1 = std::cos(angle) * identityMatrix<T,3>();
    matrix<T,3,3> term_2 = (1.0 - std::cos(angle)) * (axis * axis.transpose());

    matrix<T,3,3> antisymmetric_mat = {    0.0, axis(2), -axis(1), 
                                      -axis(2),     0.0,  axis(0), 
                                       axis(1), -axis(0),     0.0};
    matrix<T,3,3> term_3 = std::sin(angle) * antisymmetric_mat;

    return matrix<T,3,3>(term_1 + term_2 + term_3);
};

template<typename T>
matrix<T,3,3> to_dcm(const quat<T>& quaternion) {
    matrix<T, 3,3> cross_product_matrix = {0.0,           -quaternion(2),  quaternion(1),
                                           quaternion(2),            0.0, -quaternion(0), 
                                          -quaternion(1),  quaternion(0),            0.0};
    matrix<T, 3,3> term_1 = 2.0 * quaternion(3) * cross_product_matrix;
    matrix<T, 3,3> term_2 = 2.0 * cross_product_matrix.pow(2);

    return matrix<T, 3,3>(identityMatrix<T,3>() + term_1 + term_2);
};

#endif