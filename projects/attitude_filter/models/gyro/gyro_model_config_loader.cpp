#include "gyro_model_config_loader.hpp"

gyro_config load_gyro_config(const std::string& config_file_path, const uint64_t& dispersion_seed, const uint64_t& model_seed) {
    YAML::Node config_file = load_yaml_file(config_file_path);

    gyro_config config_data;

    config_data.arw_1_sigma    = get_yaml_value<double>(config_file, "angle_random_walk_1_sigma");
    config_data.rrw_1_sigma    = get_yaml_value<double>(config_file, "rate_random_walk_1_sigma");
    config_data.q_body_to_gyro = static_cast<quat<double>>(get_yaml_value<std::array<double, 4>>(config_file, "q_body_to_gyro"));
    config_data.random_seed    = model_seed;    

    double init_bias_1_sigma    = get_yaml_value<double>(config_file, "turn_on_bias_1_sigma_rps");
    double sf_1_sigma_ppm       = get_yaml_value<double>(config_file, "scale_factor_1_sigma_ppm");
    double misalign_1_sigma_rad = get_yaml_value<double>(config_file, "misalignments_1_sigma_rad");

    std::mt19937 rng(dispersion_seed);

    const double mean = 0.0;
    const double std  = 1.0;

    std::normal_distribution<> normal_distribution(mean, std);

    for (std::size_t i = 0; i < config_data.init_bias_rps.num_rows; i++) {
        config_data.init_bias_rps(i)     = init_bias_1_sigma       * normal_distribution(rng);
        config_data.scale_factors(i)     = sf_1_sigma_ppm * (1e-6) * normal_distribution(rng);
        config_data.misalignments_rad(i) = misalign_1_sigma_rad    * normal_distribution(rng);
    }

    return config_data;
}
