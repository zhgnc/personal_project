#ifndef VECTOR_UTILITIES_HPP
#define VECTOR_UTILITIES_HPP

#include "vector.hpp"

template <typename T, std::size_t length>
T& vector<T,length>::operator()(std::size_t row_to_return) {
    return matrix<T,length,1>::data[row_to_return][0];
}

template <typename T, std::size_t length>
const T& vector<T,length>::operator()(std::size_t row_to_return) const {
    return matrix<T,length,1>::data[row_to_return][0];
}

template <typename T, std::size_t length>
template<typename U>
vector<T,length>::operator vector<U,length>() const{
    vector<U,length> output;

    for (std::size_t row = 0; row < length; row++) {
        output(row) = static_cast<U>((*this)(row));
    }

    return output;
};

#endif