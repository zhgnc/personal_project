To use the matrix math library in your code #include "Matrix.hpp". Matrixes under the hood are stored as standard arrays. 
At construction a matrix object is initialized to all zeros by default but the user can pass an initializer list to set 
the matrix to specific values. One last general note is that the individual elements of a matrix can be set or retrieved 
with the () operator where the first element is an integer for the row and the second is an integer for the column. 

Here is a table of the currect functionality of the templated matrix library. 

| Function Name           | Parameters                                                       | Return Type   | Description                                     | Example Usage                                 |
|:------------------------|:-----------------------------------------------------------------|:--------------|:------------------------------------------------|:----------------------------------------------|
| matrix()                | None                                                             | matrix        | Default constructor                             | matrix<int,3,3> m;                            |
| matrix()                | std::initializer_list<type> initial_matrix                          | matrix        | Construct matrix with initializer list          | matrix<int,2,2> m = {1,2,3,4};                |
| identity_matrix()       | None                                                             | matrix        | Creates an identity matrix                      | matrix<int,3,3> m = identity_matrix<int,3>(); |
| diag_matrix()           | const matrix<type, rows, 1>& matrix or const vector<type, rows>& vec   | matrix        | Creates a diagonal matrix from the input data   | matrix<int,3,3> m = diag_matrix({1,2,3});     |
| skew_matrix()           | const matrix<type, rows, 1>& matrix or const vector<type, rows>& vec   | matrix        | Creates a skew symetric matrix A so that A^T=-A | matrix<int,3,3> m = skew_matrix<int,3>(vec);  |
| print()                 | None                                                             | void          | Prints the matrix to terminal                   | m.print();                                    |
| set_zeros()             | None                                                             | void          | Sets all elements to zero                       | m.set_zeros();                                |
| set_identity()          | None                                                             | void          | Sets the matrix to identity                     | m.set_identity();                             |
| set_skew()              | const matrix<type, rows, 1>& matrix or const vector<type, rows>& vec   | void          | Sets matrix to row major skew symetric matrix   | m.set_skew({1,2,3,4});                        |
| set_row()               | std::size_t row_index, std::initializer_list<type> row_values       | void          | Sets a specific row to given values             | m.set_row(1,{1,2,3});                         |
| set_column()            | std::size_t column_index, std::initializer_list<type> column_values | void          | Sets a specific column to given values          | m.set_column(0,{1,2,3});                      |
| trace()                 | None                                                             | type             | Returns the sum of diagonal elements            | double t = m.trace();                         |
| get_diag()              | None                                                             | matrix/vector | Returns diagonal terms of a square matrix       | matrix<double, 4,1> m = n.get_diag();         |
| set_block()             | std::size_t starting_row, std::size_t starting_column, matrix    | void          | Set block of matrix with another matrix/vector  | m.set_block(1,2,b) // m = 4x4 & b = 3x2       |
| get_block()             | std::size_t starting_row, std::size_t starting_column            | matrix/vector | Get block of elements from a matrix             | b = m.get_block(1,1) // m = 4x4 & b = 3x3     |
| operator()              | std::size_t desired_row, std::size_t desired_column              | type&            | Access element at (row,column) for modification | m(0,1) = 5;                                   |
| operator() const        | std::size_t desired_row, std::size_t desired_column              | const type&      | Access element at (row,column) for reading      | int val = m(0,1);                             |
| operator* (matrix)      | N/A                                                              | matrix        | Matrix multiplication                           | C = A * B;                                    |
| operator* (scalar)      | N/A                                                              | matrix        | Scalar multiplication                           | B = A * 2;                                    |
| operator*=              | N/A                                                              | matrix        | In-place matrix multiplication                  | A *= B;                                       |
| operator+ (matrix)      | N/A                                                              | matrix        | Matrix addition                                 | C = A + B;                                    |
| operator+ (scalar)      | N/A                                                              | matrix        | Scalar addition                                 | B = A + 5;                                    |
| operator- (matrix)      | N/A                                                              | matrix        | Matrix subtraction                              | C = A - B;                                    |
| operator- (scalar)      | N/A                                                              | matrix        | Scalar subtraction                              | B = A - 3;                                    |
| operator- (unary)       | N/A                                                              | matrix        | Negate matrix                                   | A = -A;                                       |
| operator/ (scalar)      | N/A                                                              | matrix        | Scalar division                                 | B = A / 2;                                    |
| element_wise_multiply() | const matrix<type, rows, columns>& matrix                           | matrix        | Element-wise multiplication                     | C = A.element_wise_multiply(B);               |
| element_wise_division() | const matrix<type, rows, columns>& matrix                           | matrix        | Element-wise division                           | C = A.element_wise_division(B);               |
| transpose()             | None                                                             | matrix        | Returns the transpose of the matrix             | B = A.transpose();                            |
| in_place_transpose()    | None                                                             | void          | Transposes the matrix in-place                  | A.in_place_transpose();                       |
| det()                   | None                                                             | type             | Computes the determinant                        | int val = A.det();                            |
| inv()                   | None                                                             | matrix        | Computes the inverse of the matrix              | B = A.inv();                                  |
| pow()                   | int positive_or_negative_exponent                                | matrix        | Raises the matrix to an integer power           | B = A.pow(3);                                 |
| explicit operator       | template<typename U> operator matrix<U, rows, columns>()         | matrix        | Converts matrix to a new type of same size      | B = static_cast<vector<double,3,3>(A_int);    |
