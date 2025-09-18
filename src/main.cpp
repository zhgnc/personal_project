// Copyright 2025 Zachary Hart

#include <array>
#include <iostream>

#include "math_utilities/math.hpp"
#include "sensor_models/gyro/gyro.hpp"

int main() {
  gyro g;

  // g.initialize();

  std::cout << g.frequency << "\n";

  return 0;
};
