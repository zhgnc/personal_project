#ifndef ROT_VEC_UTILITIES_HPP
#define ROT_VEC_UTILITIES_HPP

template<typename type>
type& rot_vec<type>::operator()(std::size_t row_to_return) {
    return matrix<type,3,1>::data[row_to_return][0];
};

template<typename type>
const type& rot_vec<type>::operator()(std::size_t row_to_return) const {
    return matrix<type,3,1>::data[row_to_return][0];
}; 

template<typename type>
type rot_vec<type>::angle() const {
    vector<type,3> vec = (*this);
    return vec.mag();
}; 

template<typename type>
vector<type,3> rot_vec<type>::axis() const{
    vector<type,3> vec = (*this);
    return vec.norm();
};

template<typename type>
template<typename U>
rot_vec<type>::operator rot_vec<U>() const {
    rot_vec<U> output;
    vector<type,3> vec = (*this);

    for (std::size_t row = 0; row < 3; row++) {
        output(row) = static_cast<U>(vec(row));
    }

    return output;
};

#endif