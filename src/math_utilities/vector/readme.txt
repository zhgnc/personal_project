Vectors are implemented as a specialization of the matrix class, where a vector is simply treated as a matrix with a single column. 
This inheritance means that vectors share the same underlying features of the matrix library that would be avalible to a matrix 
with 1 column. Reference ../matrix/readme.txt for list of avalible inherited capability.

To only use the vector math library in your code `#include "Vector.hpp"`. Individual elements of a vector can be set or retrieved 
with the `()` operator, where the argument is an integer representing the row index. In addition to inheriting general matrix 
functionality, the vector class introduces vector-specific math operations such as the dot product, cross product, magnitude, 
normalization, and computing the angle between two vectors.  

Here is a table of the current functionality unique to the templated vector library:

| Function Name       | Parameters                                        | Return Type | Description                                      | Example Usage                            |
|:--------------------|:--------------------------------------------------|:------------|:-------------------------------------------------|:-----------------------------------------|
| vector()            | None                                              | vector      | Construct an all zero column vector              | vector<double,3> v(m);                   |
| vector()            | std::initializer_list<T> initial_vector           | vector      | Construct a vector from an initializer list      | vector<int,3> v(m) = {1, 2, 3};          |
| operator()          | std::size_t row_index                             | T&          | Access element at row for modification           | v(0) = 5;                                |
| operator() const    | std::size_t row_index                             | const T&    | Access element at row for reading                | double val = v(1);                       |
| dot()               | const vector<T, length>& v2                       | T           | Computes the dot product of two vectors          | double d = v1.dot(v2);                   |
| cross()             | const vector<T, length>& v2                       | vector      | Computes the cross product                       | vector<double,3> c = v1.cross(v2);       |
| mag()               | None                                              | T           | Returns the magnitude (Euclidean norm)           | double m = v.mag();                      |
| norm()              | None                                              | vector      | Returns the normalized (unit) vector             | vector<double,3> u = v.norm();           |
| angle_between()     | const vector<T, length>& v2                       | T           | Computes the angle between two vectors (radians) | double angle = v1.angle_between(v2);     |
| explicit operator   | template<typename U> operator vector<U, length>() | vector<U>   | Converts vector to a new type of same size       | v = static_cast<vector<float,3>(v_int);  |

