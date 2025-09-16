The math library provides essential linear algebra and 3D rotations capabilities. The included libraries use classes to offer a variety of
functionality while storing all the data as fixed size standard arrays. The library is entirely header-only, allowing for simple inclusion 
without the need for separate compilation. A single include, `#include "math.hpp"`, provides access to all modules and utilities, making 
setup quick and straightforward. 

Each component is organized into its own folder with a dedicated readme.txt files for more specific information and a table of all the
currently avalible functions. Each folder contains a similar set of files: (1) a header aggregator like matrix.hpp, (2) a declarations file 
that defines all the classes' capability, and (3) implementation files like matrix_utilities.hpp, matrix_simple_math.hpp, and 
matrix_advanced_math.hpp where the software is actually implemented. Note, the header aggregator file can be #included instead of math.hpp
if only one library is required.


The library currently includes the following capabilties:
- **Matrix**: Matrices with support for basic utilities like accessing elements, simple math by overloading operators, and advanced operations 
              like taking the inverse of a matrix.
- **Vector**: Class is derived from the matrix class but includes additional functions like dot and cross product arithmetic.
- **Quaternion**: Represents rotations in 3D space using the Hamilton quaternion convention.
- **Rotation Vector**: Class for representing a 3D rotation as an axis-angle or rotation vector (axis * angle).
- **Transformations**: Functions and utilities to convert between quaternions, rotation vectors, and rotation/transformation matrices.