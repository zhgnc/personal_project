#ifndef STAR_TRACKER_SIM_APP_HPP
#define STAR_TRACKER_SIM_APP_HPP

#include <memory>
#include <iostream>
#include <string>
#include <cmath>

#include "../../../sim_framework/sim_includes.hpp"
#include "../../../projects/attitude_filter/models/star_tracker/star_tracker_model.hpp"

class StarTrackerSimApp : public SimAppBase {
public:
    using SimAppBase::SimAppBase;

    void configure_model(const std::string& path_to_config, SimControl& sim_ctrl) override {
        YAML::Node config_file = load_yaml_file(path_to_config);

        star_tracker_config config_data;

        config_data.x_axis_noise_rad_1_sigma = get_yaml_value<double>(config_file, "x_axis_noise_deg_1_sigma") * M_PI / 180.0;
        config_data.y_axis_noise_rad_1_sigma = get_yaml_value<double>(config_file, "y_axis_noise_deg_1_sigma") * M_PI / 180.0;
        config_data.z_axis_noise_rad_1_sigma = get_yaml_value<double>(config_file, "z_axis_noise_deg_1_sigma") * M_PI / 180.0;
        config_data.max_rate_rps             = get_yaml_value<double>(config_file, "max_rate_dps")             * M_PI / 180.0;

        config_data.q_body_to_star_tracker   = get_yaml_value<std::array<double, 4>>(config_file, "q_body_to_star_tracker");
        config_data.manual_outage_start_sec  = get_yaml_value<double>(config_file, "manual_outage_start_sec");
        config_data.manual_outage_stop_sec   = get_yaml_value<double>(config_file, "manual_outage_stop_sec");
        config_data.outages_enabled          = get_yaml_value<bool>(config_file, "outages_enabled");
        config_data.random_seed              = sim_ctrl.get_seed();

        star_tracker = StarTrackerModel(config_data);
    }

    void declare_io(IoRegistry& io) override {
        // ---- Inputs: copied into the model by the framework before each step ----
        io.subscribe("q_j2000_to_body_true", star_tracker.inputs.q_j2000_to_body_true);
        io.subscribe("body_rates_true_rps",  star_tracker.inputs.body_rates_true_rps, Units::RadiansPerSec);

        // ---- Outputs: available to every app; tlm_req vs debug_tlm sets the recording level ----
        io.tlm_req("measurement_valid",            star_tracker.outputs.star_tracker_measurement_valid);
        io.tlm_req("q_j2000_to_star_tracker_meas", star_tracker.outputs.q_j2000_to_star_tracker_meas);
        io.tlm_req("measurement_time",             star_tracker.outputs.measurement_time, Units::Seconds);

        io.debug_tlm("measurement_error_rad", star_tracker.outputs.measurement_error_rad, Units::Radians);
        io.debug_tlm("seed",                  star_tracker.outputs.seed);
    }

    void step(SimControl& sim_ctrl) override {
        // The truth attitude and rates are filled through subscribed ports
        // before this function runs
        star_tracker.inputs.current_time_sec = sim_ctrl.public_sim_data().current_sim_time_sec;

        star_tracker.run();
    }

    void teardown(SimControl& sim_ctrl) override {
        (void)sim_ctrl; // Tells compiler that I know this variable is unused
    }

private:
    StarTrackerModel star_tracker;
};

#endif
