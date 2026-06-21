#ifndef GYRO_SIM_APP_HPP
#define GYRO_SIM_APP_HPP

#include <memory>
#include <iostream>
#include <string>
#include <random>

#include "../../../sim_framework/sim_includes.hpp"
#include "../../../projects/attitude_filter/models/gyro/gyro_model.hpp"
#include "../../../projects/attitude_filter/sim_files/data_bus.hpp"

class GyroSimApp : public SimAppBase<DataBus> {
public:
    using SimAppBase::SimAppBase;

    void configure_model(const std::string& path_to_config, SimControl& sim_ctrl) override {
        // Parse yaml config for gyro model
        YAML::Node config_file = load_yaml_file(path_to_config);

        gyro_config config_data;

        config_data.arw_1_sigma    = get_yaml_value<double>(config_file, "angle_random_walk_1_sigma");
        config_data.rrw_1_sigma    = get_yaml_value<double>(config_file, "rate_random_walk_1_sigma");
        config_data.q_body_to_gyro = static_cast<quat<double>>(get_yaml_value<std::array<double, 4>>(config_file, "q_body_to_gyro"));
        config_data.random_seed    = sim_ctrl.get_seed();    

        double init_bias_1_sigma    = get_yaml_value<double>(config_file, "turn_on_bias_1_sigma_rps");
        double sf_1_sigma_ppm       = get_yaml_value<double>(config_file, "scale_factor_1_sigma_ppm");
        double misalign_1_sigma_rad = get_yaml_value<double>(config_file, "misalignments_1_sigma_rad");

        uint64_t dispersion_seed = sim_ctrl.get_seed(); // Use different seed than model to disperse gyro error states
        std::mt19937 rng(dispersion_seed);

        const double mean = 0.0;
        const double std  = 1.0;

        std::normal_distribution<> normal_distribution(mean, std);

        for (std::size_t i = 0; i < config_data.init_bias_rps.num_rows; i++) {
            config_data.init_bias_rps(i)     = init_bias_1_sigma       * normal_distribution(rng);
            config_data.scale_factors(i)     = sf_1_sigma_ppm * (1e-6) * normal_distribution(rng);
            config_data.misalignments_rad(i) = misalign_1_sigma_rad    * normal_distribution(rng);
        }
        
        // Construct gyro model with configuration data
        gyro = GyroModel(config_data);
    }

    void step(DataBus& bus, SimControl& sim_ctrl) override {
        gyro.inputs.q_j2000_to_body_true = bus.fake_dynamics_outputs.quat;
        gyro.inputs.current_time_sec     = sim_ctrl.public_sim_data().current_sim_time_sec;

        gyro.run();

        bus.gyro_outputs.measurement_valid       = gyro.outputs.gyro_measurement_valid;
        bus.gyro_outputs.measurement_time        = gyro.outputs.measurement_time;
        bus.gyro_outputs.measured_delta_angles   = gyro.outputs.measured_delta_angles;
        bus.gyro_outputs.total_delta_angle_error = gyro.outputs.total_delta_angle_error;
        bus.gyro_outputs.rate_biases             = gyro.outputs.rate_biases;
        bus.gyro_outputs.scale_factors           = gyro.outputs.scale_factors;
        bus.gyro_outputs.misalignments           = gyro.outputs.misalignments;
        bus.gyro_outputs.seed                    = gyro.outputs.seed;
    }
    
    void teardown(DataBus& bus, SimControl& sim_ctrl) override {
        (void)bus; // Tells compiler that I know these variables are unused
        (void)sim_ctrl;
    }

private:
    GyroModel gyro; 
};

#endif