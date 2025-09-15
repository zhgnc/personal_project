To use the matrix math library in your code #include "Matrix.hpp". Matrixes under the hood are stored as standard arrays. 
At construction a matrix object is initialized to all zeros by default but the user can pass an initializer list to set 
the matrix to specific values. One last general note is that the individual elements of a matrix can be set or retrieved 
with the () operator where the first element is an integer for the row and the second is an integer for the column. 

Here is a table of the currect functionality of the templated matrix library. 

| Function Name         | Parameters                                                       | Return Type  | Description                                     | Example Usage                                 |
|:----------------------|:-----------------------------------------------------------------|:-------------|:------------------------------------------------|:----------------------------------------------|
| matrix()              | None                                                             | matrix       | Default constructor                             | matrix<int,3,3> m;                            |
| matrix()              | std::initializer_list<T> initial_matrix                          | matrix       | Construct matrix with initializer list          | matrix<int,2,2> m = {1,2, 3,4};               |
| print()               | None                                                             | void         | Prints the matrix to terminal                   | m.print();                                    |
| setZeros()            | None                                                             | void         | Sets all elements to zero                       | m.setZeros();                                 |
| setIdentity()         | None                                                             | void         | Sets the matrix to identity                     | m.setIdentity();                              |
| set_row()             | std::size_t row_index, std::initializer_list<T> row_values       | void         | Sets a specific row to given values             | m.set_row(1,{1,2,3});                         |
| set_column()          | std::size_t column_index, std::initializer_list<T> column_values | void         | Sets a specific column to given values          | m.set_column(0,{1,2,3});                      |
| trace()               | None                                                             | T            | Returns the sum of diagonal elements            | double t = m.trace();                         |
| operator()            | std::size_t desired_row, std::size_t desired_column              | T&           | Access element at (row,column) for modification | m(0,1) = 5;                                   |
| operator() const      | std::size_t desired_row, std::size_t desired_column              | const T&     | Access element at (row,column) for reading      | int val = m(0,1);                             |
| operator* (matrix)    | N/A                                                              | matrix       | Matrix multiplication                           | C = A * B;                                    |
| operator* (scalar)    | N/A                                                              | matrix       | Scalar multiplication                           | B = A * 2;                                    |
| operator*=            | N/A                                                              | matrix       | In-place matrix multiplication                  | A *= B;                                       |
| operator+ (matrix)    | N/A                                                              | matrix       | Matrix addition                                 | C = A + B;                                    |
| operator+ (scalar)    | N/A                                                              | matrix       | Scalar addition                                 | B = A + 5;                                    |
| operator- (matrix)    | N/A                                                              | matrix       | Matrix subtraction                              | C = A - B;                                    |
| operator- (scalar)    | N/A                                                              | matrix       | Scalar subtraction                              | B = A - 3;                                    |
| operator- (unary)     | N/A                                                              | matrix       | Negate matrix                                   | A = -A;                                       |
| operator/ (scalar)    | N/A                                                              | matrix       | Scalar division                                 | B = A / 2;                                    |
| elementWiseMultiply() | const matrix<T, rows, columns>& matrix                           | matrix       | Element-wise multiplication                     | C = A.elementWiseMultiply(B);                 |
| elementWiseDivision() | const matrix<T, rows, columns>& matrix                           | matrix       | Element-wise division                           | C = A.elementWiseDivision(B);                 |
| transpose()           | None                                                             | matrix       | Returns the transpose of the matrix             | B = A.transpose();                            |
| inPlaceTranspose()    | None                                                             | void         | Transposes the matrix in-place                  | A.inPlaceTranspose();                         |
| det()                 | None                                                             | T            | Computes the determinant                        | int val = A.det();                            |
| inv()                 | None                                                             | matrix       | Computes the inverse of the matrix              | B = A.inv();                                  |
| pow()                 | int positive_or_negative_exponent                                | matrix       | Raises the matrix to an integer power           | B = A.pow(3);                                 |
| identityMatrix()      | Type, int size                                                   | matrix       | Used to initialize a matrix to identity         | matrix<int,3,3> A = identityMatrix<int,3>();  |

