#ifndef ROT_VEC_UTILITIES_HPP
#define ROT_VEC_UTILITIES_HPP

template<typename T>
T& rot_vec<T>::operator()(std::size_t row_to_return) {
    return matrix<T,3,1>::data[row_to_return][0];
};

template<typename T>
const T& rot_vec<T>::operator()(std::size_t row_to_return) const {
    return matrix<T,3,1>::data[row_to_return][0];
}; 

template<typename T>
T rot_vec<T>::angle() const {
    vector<T,3> vec = (*this);
    return vec.mag();
}; 

template<typename T>
vector<T,3> rot_vec<T>::axis() const{
    vector<T,3> vec = (*this);
    return vec.norm();
};

#endif