#ifndef ROT_VEC_CONSTRUCTORS_HPP
#define ROT_VEC_CONSTRUCTORS_HPP

template<typename type>
rot_vec<type>::rot_vec(const matrix<type,3,1>& mat) {
    for (std::size_t row = 0; row < 3; row++) {
        (*this)(row) = mat(row,0);
    }
};

template<typename type>
rot_vec<type>::rot_vec(const vector<type,3>& axis, type angle) {
    vector<type,3> unit_axis = axis.norm();
    (*this) = unit_axis * angle;
}; 

#endif