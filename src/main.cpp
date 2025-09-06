// Copyright 2025 Zachary Hart

#include <array>
#include <iostream>

#include <math_utilities/matrix.hpp>
#include <math_utilities/vector.hpp>

template <typename T> T pi(3.1415926535);

int main() {
  // matrix<int, 3,3> I3 = identityMatrix<int,3>();
  // vector<int, 3>   v1;

  // I3.print();
  // v1.print();

  // vector<int, 3> v2 = I3 * v1;
  // v2.print();

  // v2 = v2 + 12;
  // v2.print();

  // v2 = v2 - v1;

  // int dot_output = v2.dot(v1+3);
  // std::cout << dot_output << "\n\n";

  // vector<int, 3> v3 = {1,2,3};
  // vector<int, 3> v4 = {4,120,2000};

  // vector<int, 3> cross_output = v3.cross(v4);
  // cross_output.print();

  vector<double, 2> v1;
  vector<double, 2> v2 = {1, 0};
  double angle = 0.0;
  double ang_out = 0;
  double rad2deg = 57.2958;
  double deg2rad = 1.0 / rad2deg;

  for (std::size_t i = 0; i < 360; i++) {
    angle = i * deg2rad;
    v1 = {std::cos(angle), std::sin(angle)};
    // v1.print();

    ang_out = v1.angle_between(v2);
    std::cout << ang_out * rad2deg << "\n";
  }

  return 0;
};
