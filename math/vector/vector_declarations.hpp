#ifndef VECTOR_DECLARATIONS_HPP
#define VECTOR_DECLARATIONS_HPP

#include "../matrix/matrix.hpp"
#include "vector.hpp"

template <typename type, std::size_t length>
class vector : public matrix<type, length, 1> 
{
public:
    // Constructors
    using matrix<type, length, 1>::matrix; // Inherit matrix class constructors 
    vector(const matrix<type, length, 1>& mat); 

    // Utilities
    type& operator()(std::size_t row_to_return);
    const type& operator()(std::size_t row_to_return) const;

    template<typename U>
    explicit operator vector<U,length>() const;

    // Simple Math
    type dot(const vector<type, length>& v2) const;
    vector<type, length> cross(const vector<type,length>& v2) const;
    type mag() const;
    vector<type, length> norm() const;
    type angle_between(const vector<type,length>& v2) const;
};


#endif