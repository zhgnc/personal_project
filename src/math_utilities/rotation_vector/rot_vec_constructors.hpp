#ifndef ROT_VEC_CONSTRUCTORS_HPP
#define ROT_VEC_CONSTRUCTORS_HPP

template<typename T>
rot_vec<T>::rot_vec(const matrix<T,3,1>& mat) {
    for (std::size_t row = 0; row < 3; row++) {
        (*this)(row) = mat(row,0);
    }
};

template<typename T>
rot_vec<T>::rot_vec(const vector<T,3>& axis, T angle) {
    vector<T,3> unit_axis = axis.norm();
    (*this) = unit_axis * angle;
}; 

#endif