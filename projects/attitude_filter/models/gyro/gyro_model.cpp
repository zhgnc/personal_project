#include "gyro_model.hpp"

GyroModel::GyroModel(const gyro_config& config_data) {
  config = config_data;

  config.q_body_to_gyro = config.q_body_to_gyro.normalize();
  
  rate_bias = config.init_bias_rps;
  sf_misalign_matrix = diag_matrix<double, 3>(config.scale_factors) + skew_matrix<double, 3>({-config.misalignments_rad(2), config.misalignments_rad(1), -config.misalignments_rad(0)});
  
  gyro_meas_valid = false;
  first_cycle     = true;

  const double mean = 0.0;
  const double std  = 1.0;
  
  rng.seed(config.random_seed);
  normal_distribution = std::normal_distribution<>(mean, std);

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

    meas_delta_angles.set_zeros();
    return;
  };

  q_prev_to_now     = (q_j2000_to_body_now * q_j2000_to_body_prev.inv()).normalize();
  true_delta_angles = to_rot_vec(q_prev_to_now);
  true_delta_angles = config.q_body_to_gyro * true_delta_angles;

  dt            = time_now_sec - time_prev_sec;
  time_prev_sec = time_now_sec;

  for (std::size_t i = 0; i < arw_error.num_rows; i++) {
    arw_error(i)  = config.arw_1_sigma * std::sqrt(dt) * normal_distribution(rng);
    rate_bias(i) += config.rrw_1_sigma * std::sqrt(dt) * normal_distribution(rng);
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
  outputs.scale_factors           = config.scale_factors;
  outputs.misalignments           = config.misalignments_rad;
  outputs.seed                    = config.random_seed;
};