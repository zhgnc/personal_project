A rotation vector (also known as an axis-angle representation) describes a 3D rotation by storing a unit vector for the rotation axis, 
multiplied by the angle of rotation in radians. Under the hood, the rotation vector class inherits from the matrix class as a fixed-size 
`3x1` column matrix.  
 

To only use the rotation vector library in your code `#include "rot_vec.hpp"`. At construction, a rotation vector can either be 
initialized directly from a `3x1` matrix, or from an explicit axis and angle pair. Individual elements can be accessed using the `()` 
operator with a row index (just like vectors).  

In addition to general matrix/vector functionality, the rotation vector class provides utilities to extract the angle, compute the 
normalized axis of rotation, and convert between element types.  

Here is a table of the current functionality unique to the rotation vector library:

| Function Name       | Parameters                                  | Return Type | Description                                         | Example Usage                                      |
|:--------------------|:--------------------------------------------|:------------|:----------------------------------------------------|:---------------------------------------------------|
| rot_vec()           | None                                        | rot_vec     | Construct a rotation vector of all zeros            | rot_vec<double> r;                                 |
| rot_vec()           | const matrix<T,3,1>& mat                    | rot_vec     | Construct a rotation vector from a 3×1 matrix       | rot_vec<double> r = matrix_3_by_1;                 |
| rot_vec()           | std::initializer_list<T> initial_rot_vec    | rot_vec     | Construct a rotation vector from an initalizer list | rot_vec<double> r = {0.3, 0.2, 0.1};               |
| rot_vec()           | const vector<T,3>& axis, T angle            | rot_vec     | Construct from an axis-angle representation         | rot_vec<double> r = {axis, angle};                 |
| operator()          | std::size_t row_index                       | T&          | Access element at row for modification              | r(0) = 0.5;                                        |
| operator() const    | std::size_t row_index                       | const T&    | Access element at row for reading                   | double val = r(2);                                 |
| angle()             | None                                        | T           | Returns the angle of rotation in radians            | double angle = r.angle();                          |
| axis()              | None                                        | vector<T,3> | Returns the normalized axis of rotation             | vector<double,3> a = r.axis();                     |
| explicit operator   | template<typename U> operator rot_vec<U>()  | rot_vec<U>  | Converts rotation vector to a new type              | r = static_cast<rot_vec<float>>(rot_vec_int);      |

