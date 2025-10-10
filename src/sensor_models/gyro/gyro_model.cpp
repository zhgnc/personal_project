#ifndef GYRO_CPP
#define GYRO_CPP

#include "gyro_model.hpp"
#include "struct_defs.hpp"
#include "yaml-cpp/yaml.h"

GyroModel::GyroModel() {
  init_bias_1_sigma = default_config.turn_on_bias_1_sigma;
  arw_1_sigma       = default_config.angle_random_walk_1_sigma;
  rrw_1_sigma       = default_config.rate_random_walk_1_sigma;
  sf_1_sigma        = default_config.scale_factor_1_sigma;
  misalign_1_sigma  = default_config.misalignment_1_sigma;
  frequency         = default_config.rate_hz;
  random_seed       = default_config.random_seed;
};

GyroModel::GyroModel(const std::string &config_file) {
  YAML::Node config_data = YAML::LoadFile(config_file);

  init_bias_1_sigma = config_data["turn_on_bias_1_sigma"].as<double>();
  arw_1_sigma       = config_data["angle_random_walk_1_sigma"].as<double>();
  rrw_1_sigma       = config_data["rate_random_walk_1_sigma"].as<double>();
  sf_1_sigma        = config_data["scale_factor_1_sigma"].as<double>();
  misalign_1_sigma  = config_data["misalignment_1_sigma"].as<double>();
  frequency         = config_data["rate_hz"].as<double>();
  random_seed       = config_data["random_seed"].as<double>();
};

void GyroModel::initialize() {
  const double mean = 0.0;
  const double std = 1.0;

  rng.seed(random_seed);
  normal_distribution = std::normal_distribution<>(mean, std);

  for (std::size_t i = 0; i < initial_rate_biases.num_rows; i++) {
    initial_rate_biases(i) = init_bias_1_sigma * normal_distribution(rng);
    bias_error(i) = initial_rate_biases(i);

    scale_factors(i) = sf_1_sigma * normal_distribution(rng);
    misalignments(i) = misalign_1_sigma * normal_distribution(rng);
  }

  sf_misalign_matrix = {scale_factors(0),  -misalignments(2), misalignments(1),
                        misalignments(2),  scale_factors(1),  -misalignments(0),
                        -misalignments(1), misalignments(0),  scale_factors(2)};
  I3.setIdentity();
  dt = 1 / frequency;

  gyro_meas_valid = false;
  first_cycle = true;
};

void GyroModel::copy_inputs_to_class() {
  q_j2000_to_body_now = inputs.q_j2000_to_body_true;
};

void GyroModel::execute() {
  if (first_cycle == true) {
    first_cycle = false;
    q_j2000_to_body_prev = q_j2000_to_body_now;
    meas_delta_angles.setZeros();
    return;
  };

  q_prev_to_now = q_j2000_to_body_now * q_j2000_to_body_prev.inv();
  true_delta_angles = to_rot_vec(q_prev_to_now);

  for (std::size_t i = 0; i < arw_error.num_rows; i++) {
    arw_error(i) = arw_1_sigma * std::sqrt(dt) * normal_distribution(rng);
    bias_error(i) += rrw_1_sigma * std::sqrt(dt) * normal_distribution(rng);
  }

  meas_delta_angles = (I3 + sf_misalign_matrix) * true_delta_angles +
                      bias_error * dt + arw_error;

  q_j2000_to_body_prev = q_j2000_to_body_now;
  gyro_meas_valid = true;
};

void GyroModel::set_outputs() {
  outputs.gyro_measurement_valid = gyro_meas_valid;
  outputs.measured_delta_angles = meas_delta_angles;
};

#endif