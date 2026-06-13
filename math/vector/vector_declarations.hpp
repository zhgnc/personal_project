#ifndef VECTOR_DECLARATIONS_HPP
#define VECTOR_DECLARATIONS_HPP

#include "../matrix/matrix.hpp"
#include "vector.hpp"

template <typename T, std::size_t length>
class vector : public matrix<T, length, 1> 
{
public:
    // Constructors
    using matrix<T, length, 1>::matrix; // Inherit matrix class constructors 
    vector(const matrix<T, length, 1>& mat); 

    // Utilities
    T& operator()(std::size_t row_to_return);
    const T& operator()(std::size_t row_to_return) const;

    template<typename U>
    explicit operator vector<U,length>() const;

    // Simple Math
    T dot(const vector<T, length>& v2) const;
    vector<T, length> cross(const vector<T,length>& v2) const;
    T mag() const;
    vector<T, length> norm() const;
    T angle_between(const vector<T,length>& v2) const;
};


#endif