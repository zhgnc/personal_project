// Copyright 2025 Zachary Hart

#include <array>
#include <iostream>
#include <filesystem>

#include "math_utilities/math.hpp"
#include "sensor_models/gyro/gyro.hpp"

int main() {
  gyro g("src/config_files/gyro_config.yaml");
  g.initialize();

  for (std::size_t i = 0; i < 5; i++) {
    g.inputs.q_j2000_to_body_true.setIdentity();
    g.copy_inputs_to_class();
    g.execute();
    g.set_outputs();

    g.outputs.measured_delta_angles.print();
  }
  
  // std::cout << g.frequency << "\n";

  return 0;
};
