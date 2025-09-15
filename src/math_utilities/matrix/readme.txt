To use the matrix math library in your code #include "Matrix.hpp". Matrixes under the hood are stored as standard arrays. 
At construction a matrix object is initialized to all zeros by default but the user can pass an initializer list to set 
the matrix to specific values. 

The individual elements of a matrix can be set or retrieved with the () operator where the first element is an integer 
for the row and the second is an integer for the column. 


| Function Name    | Parameters                                                       | Return Type  | Description                                            | Example Usage                  |
|:-----------------|:-----------------------------------------------------------------|:-------------|:-------------------------------------------------------|:-------------------------------|
| matrix()         | None                                                             | matrix       | Default constructor                                    | matrix<int,3,3> m;             |
| matrix()         | std::initializer_list<T> initial_matrix                          | matrix       | Construct matrix with initializer list                 | matrix<int,2,2> m = {1,2,3,4}; |
| print()          | None                                                             | void         | Prints the matrix to standard output                   | m.print();                     |
| setZeros()       | None                                                             | void         | Sets all elements to zero                              | m.setZeros();                  |
| setIdentity()    | None                                                             | void         | Sets the matrix to identity (only for square matrices) | m.setIdentity();               |
| set_row()        | std::size_t row_index, std::initializer_list<T> row_values       | void         | Sets a specific row to given values                    | m.set_row(1,{1,2,3});          |
| set_column()     | std::size_t column_index, std::initializer_list<T> column_values | void         | Sets a specific column to given values                 | m.set_column(0,{1,2,3});       |
| trace()          | None                                                             | T            | Returns the sum of the diagonal elements               | T t = m.trace();               |
| operator()       | std::size_t desired_row, std::size_t desired_column              | T&           | Access element at (row,column) for modification        | m(0,1) = 5;                    |
| operator() const | std::size_t desired_row, std::size_t desired_column              | const T&     | Access element at (row,column) for reading             | T val = m(0,1);                |
