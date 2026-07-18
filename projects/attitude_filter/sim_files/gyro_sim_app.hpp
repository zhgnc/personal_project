#ifndef GYRO_SIM_APP_HPP
#define GYRO_SIM_APP_HPP

#include <memory>
#include <iostream>
#include <string>
#include <random>

#include "../../../sim_framework/sim_includes.hpp"
#include "../../../projects/attitude_filter/models/gyro/gyro_model.hpp"

class GyroSimApp : public SimAppBase {
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

        for (std::size_t i = 0; i < config_data.init_bias_rps.num_rows; i++) {
            config_data.init_bias_rps(i)     = init_bias_1_sigma       * sim_ctrl.sample_normal(0.0, 1.0);
            config_data.scale_factors(i)     = sf_1_sigma_ppm * (1e-6) * sim_ctrl.sample_uniform(-1.0, 1.0);
            config_data.misalignments_rad(i) = misalign_1_sigma_rad    * sim_ctrl.sample_normal(0.0, 1.0);
        }

        // Construct gyro model with configuration data
        gyro = GyroModel(config_data);
    }

    void declare_io(IoRegistry& io) override {
        // ---- Inputs: copied into the model by the framework before each step ----
        io.subscribe("q_j2000_to_body_true", gyro.inputs.q_j2000_to_body_true);

        // ---- Outputs: available to every app; tlm_req vs debug_tlm sets the recording level ----
        io.tlm_req("measurement_valid",     gyro.outputs.gyro_measurement_valid);
        io.tlm_req("measurement_time",      gyro.outputs.measurement_time,      Units::Seconds);
        io.tlm_req("measured_delta_angles", gyro.outputs.measured_delta_angles, Units::Radians);

        io.debug_tlm("total_delta_angle_error", gyro.outputs.total_delta_angle_error, Units::Radians);
        io.debug_tlm("rate_biases",             gyro.outputs.rate_biases,             Units::RadiansPerSec);
        io.debug_tlm("scale_factors",           gyro.outputs.scale_factors,           Units::Dimensionless);
        io.debug_tlm("misalignments",           gyro.outputs.misalignments,           Units::Radians);
        io.debug_tlm("seed",                    gyro.outputs.seed);
    }

    void step(SimControl& sim_ctrl) override {
        // gyro.inputs.q_j2000_to_body_true is filled through its subscribed
        // port before this function runs
        gyro.inputs.current_time_sec = sim_ctrl.public_sim_data().current_sim_time_sec;

        gyro.run();
    }

    void teardown(SimControl& sim_ctrl) override {
        (void)sim_ctrl; // Tells compiler that I know this variable is unused
    }

private:
    GyroModel gyro;
};

#endif
