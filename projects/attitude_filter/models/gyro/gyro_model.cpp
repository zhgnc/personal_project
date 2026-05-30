#include "gyro_model.hpp"

GyroModel::GyroModel(const std::string &config_file, uint64_t seed) {
  YAML::Node config_data = load_yaml_file(config_file);

  arw_1_sigma            = get_yaml_value<double>(config_data, "angle_random_walk_deg_1_sigma") * deg2rad;
  init_rate_bias_1_sigma = get_yaml_value<double>(config_data, "turn_on_rate_bias_deg_1_sigma") * deg2rad;
  rrw_1_sigma            = get_yaml_value<double>(config_data, "rate_random_walk_deg_1_sigma")  * deg2rad;
  sf_1_sigma             = get_yaml_value<double>(config_data, "scale_factor_ppm_1_sigma")      * 1e-6;
  misalign_1_sigma       = get_yaml_value<double>(config_data, "misalignment_deg_1_sigma")      * deg2rad;
  q_body_to_gyro         = get_yaml_value<std::array<double, 4>>(config_data, "q_body_to_gyro");
  random_seed            = seed;

  initialize();
};

void GyroModel::initialize() {
  const double mean = 0.0;
  const double std  = 1.0;

  rng.seed(random_seed);
  normal_distribution = std::normal_distribution<>(mean, std);

  for (std::size_t i = 0; i < rate_bias.num_rows; i++) {
    rate_bias(i)     = init_rate_bias_1_sigma * normal_distribution(rng);
    scale_factors(i) = sf_1_sigma             * normal_distribution(rng);
    misalignments(i) = misalign_1_sigma       * normal_distribution(rng);
  }

  sf_misalign_matrix = {scale_factors(0), -misalignments(2),  misalignments(1),
                        misalignments(2),  scale_factors(1), -misalignments(0),
                       -misalignments(1),  misalignments(0),  scale_factors(2)};
  I3.setIdentity();

  gyro_meas_valid = false;
  first_cycle     = true;
};

void GyroModel::run() {
    GyroModel::copy_inputs_to_class(); 
    GyroModel::execute(); 
    GyroModel::set_outputs();
}

void GyroModel::copy_inputs_to_class() {
  q_j2000_to_body_now = inputs.q_j2000_to_body_true;
  time_now_sec        = inputs.current_time_sec;
};

void GyroModel::execute() {
  if (first_cycle == true) {
    first_cycle          = false;
    q_j2000_to_body_prev = q_j2000_to_body_now;
    time_prev_sec        = time_now_sec;

    meas_delta_angles.setZeros();
    return;
  };

  q_prev_to_now     = (q_j2000_to_body_now * q_j2000_to_body_prev.inv()).normalize();
  true_delta_angles = to_rot_vec(q_prev_to_now);
  true_delta_angles = q_body_to_gyro * true_delta_angles;

  dt            = time_now_sec - time_prev_sec;
  time_prev_sec = time_now_sec;

  for (std::size_t i = 0; i < arw_error.num_rows; i++) {
    arw_error(i)  = arw_1_sigma * std::sqrt(dt) * normal_distribution(rng);
    rate_bias(i) += rrw_1_sigma * std::sqrt(dt) * normal_distribution(rng);
  }

  meas_delta_angles = (I3 + sf_misalign_matrix) * true_delta_angles + (rate_bias * dt) + arw_error;

  total_delta_angle_error = true_delta_angles - meas_delta_angles;
  q_j2000_to_body_prev    = q_j2000_to_body_now;
  gyro_meas_valid         = true;
};

void GyroModel::set_outputs() {
  outputs.gyro_measurement_valid  = gyro_meas_valid;
  outputs.measurement_time        = time_now_sec;
  outputs.measured_delta_angles   = meas_delta_angles;
  outputs.total_delta_angle_error = total_delta_angle_error;
  outputs.rate_biases             = rate_bias;
  outputs.scale_factors           = scale_factors;
  outputs.misalignments           = misalignments;
  outputs.seed                    = random_seed;
};