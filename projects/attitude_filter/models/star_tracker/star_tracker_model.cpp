#include "star_tracker_model.hpp"

StarTrackerModel::StarTrackerModel(const std::string &config_file, uint64_t seed) {
  YAML::Node config_data = load_yaml_file(config_file);
  
  x_axis_noise_rad_1_sigma = get_yaml_value<double>(config_data, "x_axis_noise_deg_1_sigma") * deg2rad;
  y_axis_noise_rad_1_sigma = get_yaml_value<double>(config_data, "y_axis_noise_deg_1_sigma") * deg2rad;
  z_axis_noise_rad_1_sigma = get_yaml_value<double>(config_data, "z_axis_noise_deg_1_sigma") * deg2rad;
  max_rate_rps             = get_yaml_value<double>(config_data, "max_rate_dps")             * deg2rad;

  q_body_to_star_tracker   = get_yaml_value<std::array<double, 4>>(config_data, "q_body_to_star_tracker");
  random_seed              = seed;

  initialize();
};

void StarTrackerModel::initialize() {
  const double mean = 0.0;
  const double std  = 1.0;

  rng.seed(random_seed);
  normal_distribution = std::normal_distribution<>(mean, std);

  star_tracker_meas_valid    = false;
  per_axis_noise_rad_1_sigma = {x_axis_noise_rad_1_sigma, 
                                y_axis_noise_rad_1_sigma, 
                                z_axis_noise_rad_1_sigma};
};

void StarTrackerModel::run() {
    StarTrackerModel::copy_inputs_to_class(); 
    StarTrackerModel::execute(); 
    StarTrackerModel::set_outputs();
}

void StarTrackerModel::copy_inputs_to_class() {
  true_q_j2000_to_body = inputs.q_j2000_to_body_true;
  true_body_rates_rps  = inputs.body_rates_true_rps;
  time_now_sec         = inputs.current_time_sec;
};

void StarTrackerModel::execute() {
  if (true_body_rates_rps.mag() > max_rate_rps) {
    q_j2000_to_star_tracker_meas.setIdentity();
    noise.setZeros();
    star_tracker_meas_valid = false;
    return;
  }

  for (std::size_t i = 0; i < per_axis_noise_rad_1_sigma.num_rows; i++) {
    noise(i) = per_axis_noise_rad_1_sigma(i) * normal_distribution(rng);
  }

  q_noise                      = to_quat(noise);
  q_j2000_to_star_tracker_meas = q_noise * q_body_to_star_tracker * true_q_j2000_to_body;
  star_tracker_meas_valid      = true;
};

void StarTrackerModel::set_outputs() {
  outputs.star_tracker_measurement_valid = star_tracker_meas_valid;
  outputs.q_j2000_to_star_tracker_meas   = q_j2000_to_star_tracker_meas;
  outputs.measurement_error_rad          = noise;
  outputs.seed                           = random_seed;
};