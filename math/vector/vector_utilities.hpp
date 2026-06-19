#ifndef VECTOR_UTILITIES_HPP
#define VECTOR_UTILITIES_HPP

#include "vector.hpp"

template <typename type, std::size_t length>
type& vector<type,length>::operator()(std::size_t row_to_return) {
    return matrix<type,length,1>::data[row_to_return][0];
}

template <typename type, std::size_t length>
const type& vector<type,length>::operator()(std::size_t row_to_return) const {
    return matrix<type,length,1>::data[row_to_return][0];
}

template <typename type, std::size_t length>
template<typename U>
vector<type,length>::operator vector<U,length>() const{
    vector<U,length> output;

    for (std::size_t row = 0; row < length; row++) {
        output(row) = static_cast<U>((*this)(row));
    }

    return output;
};

#endif