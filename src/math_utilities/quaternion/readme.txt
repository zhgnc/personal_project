This class implements quaternions as fixed-size `4x1` standard arrays following the Hamilton convention as described in 
Zanetti's paper "Rotations, Transformations, Left Quaternions, Right Quaternions?". In this paper the Hamilton quaternion 
is represented as q = (x, y, z, w), where (x, y, z) are the vector part of the quaternion and w is the scalar portion. 

Note, according to this convention quaternion multiplication is active where quaternions are multiplied in the same order
as matrices. For example if the quaternion rotating from frame 1 to from 3 is desired, the following multiplication order 
should be followed: q_1_to_3 = q_2_to_3 * q_1_to_2. Also rotating a vector by a quaternion simply done with the syntax 
v_new = q * v. The default rotation is active which is defined as (v_new, 0) = q * (v, 0) * (q*). If a passive rotation is
desired, (v_new, 0) = (q*) * (v, 0) * q, use the syntax v_new = q.inv() * v.

To only use the quaternion library, #include "quaternion.hpp" and here is a table of the current functionality of the 
quaternion library:

| Function Name       | Parameters                              | Return Type  | Description                                                        | Example Usage                           |
|:--------------------|:----------------------------------------|:-------------|:-------------------------------------------------------------------|:----------------------------------------|
| quat()              | None                                    | quat         | Default constructor, initializes to identity quaternion            | quat<double> q;                         |
| quat()              | std::initializer_list<T> initial_quat   | quat         | Construct quaternion with initializer list                         | quat<double> q = {0, 0, 0, 1};          |
| quat()              | const quat<T>& another_quaternion       | quat         | Construct quaternion and set it equal to another quaternion        | quat<double> q2 = q1;                   |
| operator()          | std::size_t element_to_return           | T&           | Access element by index for modification                           | q(0) = 0.5;                             |
| operator() const    | std::size_t element_to_return           | const T&     | Access element by index for reading                                | double scalar = q(3);                   |
| operator=           | const quat<T>& another_quaternion       | quat&        | Assignment operator                                                | q1 = q2;                                |
| print()             | None                                    | void         | Prints quaternion to terminal                                      | q.print();                              |
| setIdentity()       | None                                    | void         | Sets quaternion to identity (0,0,0,1)                              | q.setIdentity();                        |
| scalar()            | None                                    | T            | Returns the scalar (real) part                                     | double scalar = q.scalar();             |
| vec()               | None                                    | vector<T,3>  | Returns the vector (imaginary) part                                | vector<double,3> v = q.vec();           |
| setVector()         | vector<T,3> vector                      | void         | Sets the vector (imaginary) part                                   | q.setVector(v);                         |
| setScalar()         | T scalar                                | void         | Sets the scalar (real) part                                        | q.setScalar(1.0);                       |
| neg()               | None                                    | quat         | Returns the negated quaternion                                     | quat<double> q_neg = q.neg();           |
| normalize()         | None                                    | quat         | Returns a normalized version of the quaternion                     | q = q.normalize();                      |
| conjugate()         | None                                    | quat         | Returns the conjugate quaternion                                   | q = q.conjugate();                      |
| inv()               | None                                    | quat         | Returns the inverse quaternion (for unit quaternions = conjugate)  | q = q.inv();                            |
| operator*           | const quat<T>& q_12                     | quat         | Quaternion multiplication (active rotation composition)            | q_13 = q_23 * q_12;                     |
| operator*           | const vector<T,3>& vector               | vector<T,3>  | Rotates a 3D vector using the quaternion                           | vector<double,3> v2 = q * v1;           |
| explicit operator   | template<typename U> operator quat<U>() | quat         | Converts quaternion to a new type                                  | q = static_cast<quat<double>(q_float);  |
