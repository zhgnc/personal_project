#ifndef ROT_VEC_DECLARATIONS_HPP
#define ROT_VEC_DECLARATIONS_HPP

#include "../matrix/matrix.hpp"
#include "../vector/vector.hpp"

template<typename type>
class rot_vec : public matrix<type,3,1> 
{    
  public:
    
    // Constructors
    using matrix<type,3,1>::matrix; // Inherit matrix class constructors 
    rot_vec(const matrix<type,3,1>& mat);
    rot_vec(const vector<type,3>& axis, type angle); 
    
    // Utilities
    type& operator()(std::size_t row_to_return);
    const type& operator()(std::size_t row_to_return) const; 
    type angle() const; 
    vector<type,3> axis() const; 
    
    template<typename U>
    explicit operator rot_vec<U>() const;
};

#endif