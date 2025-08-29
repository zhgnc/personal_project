// Copyright 2025 Zachary Hart

#include <array>
#include <iostream>

#include <math_utilities/matrix2D.hpp>
#include <math_utilities/vector.hpp>

template <typename T> T pi(3.1415926535);

int main() { 
    std::cout << "Test 1: \n";
    Matrix2D<double, 2,2> A = {1,2, 3,4};
    Matrix2D<double, 2,2> B = {5,6, 7,8};
    Matrix2D<double, 2,2> C = {9,10, 11,12};
    // Matrix2D<double, 2,2> D = {13,14, 15,16};
    
    Matrix2D<double, 2,2> output = C*B*A;
    output.print();
    Matrix2D<double, 2,2> expected_output = {517,776, 625,926};
    expected_output.print();



    std::cout << "Test 2: \n";
    output = (C*B)*A;
    output.print();
    expected_output = {517,766, 625,926};
    expected_output.print();

    
    return 0; 
}
