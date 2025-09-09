#ifndef ROT_VEC_DECLARATIONS_HPP
#define ROT_VEC_DECLARATIONS_HPP

#include "../matrix/matrix.hpp"
#include "../vector/vector.hpp"

template<typename T>
class rot_vec : public matrix<T,3,1> 
{    
  public:
    
    // Constructors
    using matrix<T,3,1>::matrix; // Inherit matrix class constructors 
    rot_vec(const matrix<T,3,1>& mat);
    rot_vec(const vector<T,3>& axis, T angle); 
    
    // Utilities
    T& operator()(std::size_t row_to_return);
    const T& operator()(std::size_t row_to_return) const; 
    T angle() const; 
    vector<T,3> axis() const; 
};

#endif