#include "star_tracker_model.hpp"

StarTrackerModel::StarTrackerModel(const star_tracker_config& config_data) {
  config = config_data;

  config.q_body_to_star_tracker = config.q_body_to_star_tracker.normalize();

  const double mean = 0.0;
  const double std  = 1.0;

  rng.seed(config.random_seed);
  normal_distribution = std::normal_distribution<>(mean, std);

  star_tracker_meas_valid    = false;
  per_axis_noise_rad_1_sigma = {config.x_axis_noise_rad_1_sigma, 
                                config.y_axis_noise_rad_1_sigma, 
                                config.z_axis_noise_rad_1_sigma};
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
  bool max_rate_exceeded    = true_body_rates_rps.mag() > config.max_rate_rps;
  bool manual_outage_active = time_now_sec > config.manual_outage_start_sec && time_now_sec < config.manual_outage_stop_sec;
  
  if (config.outages_enabled && (max_rate_exceeded || manual_outage_active)) {
    q_j2000_to_star_tracker_meas.set_identity();
    noise.set_zeros();
    star_tracker_meas_valid = false;
    return;
  }

  for (std::size_t i = 0; i < per_axis_noise_rad_1_sigma.num_rows; i++) {
    noise(i) = per_axis_noise_rad_1_sigma(i) * normal_distribution(rng);
  }

  q_noise                      = to_quat(noise);
  q_j2000_to_star_tracker_meas = (q_noise * config.q_body_to_star_tracker * true_q_j2000_to_body).normalize();
  star_tracker_meas_valid      = true;
};

void StarTrackerModel::set_outputs() {
  outputs.star_tracker_measurement_valid = star_tracker_meas_valid;
  outputs.q_j2000_to_star_tracker_meas   = q_j2000_to_star_tracker_meas;
  outputs.measurement_error_rad          = noise;
  outputs.measurement_time               = time_now_sec;
  outputs.seed                           = config.random_seed;
};