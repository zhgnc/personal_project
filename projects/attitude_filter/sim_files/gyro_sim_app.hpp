#ifndef GYRO_SIM_APP_HPP
#define GYRO_SIM_APP_HPP

#include <string>

#include "../../../sim_framework/sim_includes.hpp"
#include "../../../projects/attitude_filter/models/gyro/gyro_model.hpp"

class GyroSimApp : public SimAppBase {
public:
    using SimAppBase::SimAppBase;

    void configure_model(const std::string& path_to_config, SimControl& sim_ctrl) override {
        YAML::Node app_config = load_yaml_file(path_to_config)["app_config"];

        // Parse yaml config for gyro model
        gyro_config config_data;

        config_data.arw_1_sigma    = get_yaml_value<double>(app_config, "angle_random_walk_1_sigma");
        config_data.rrw_1_sigma    = get_yaml_value<double>(app_config, "rate_random_walk_1_sigma");
        config_data.q_body_to_gyro = static_cast<quat<double>>(get_yaml_value<std::array<double, 4>>(app_config, "q_body_to_gyro"));
        config_data.random_seed    = sim_ctrl.get_seed();

        double init_bias_1_sigma    = get_yaml_value<double>(app_config, "turn_on_bias_1_sigma_rps");
        double sf_1_sigma_ppm       = get_yaml_value<double>(app_config, "scale_factor_1_sigma_ppm");
        double misalign_1_sigma_rad = get_yaml_value<double>(app_config, "misalignments_1_sigma_rad");

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
        io.sub("q_j2000_to_body_true", gyro.inputs.q_j2000_to_body_true);

        // ---- Outputs: available to every app; tlm_req vs tlm_debug sets the recording level ----
        io.tlm_req("measurement_valid",     gyro.outputs.gyro_measurement_valid);
        io.tlm_req("measurement_time",      gyro.outputs.measurement_time);
        io.tlm_req("measured_delta_angles", gyro.outputs.measured_delta_angles);

        io.tlm_debug("total_delta_angle_error", gyro.outputs.total_delta_angle_error);
        io.tlm_debug("rate_biases",             gyro.outputs.rate_biases);
        io.tlm_debug("scale_factors",           gyro.outputs.scale_factors);
        io.tlm_debug("misalignments",           gyro.outputs.misalignments);
        io.tlm_debug("seed",                    gyro.outputs.seed);
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
