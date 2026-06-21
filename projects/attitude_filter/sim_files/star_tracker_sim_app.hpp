#ifndef STAR_TRACKER_SIM_APP_HPP
#define STAR_TRACKER_SIM_APP_HPP

#include <memory>
#include <iostream>
#include <string>
#include <cmath>

#include "../../../sim_framework/sim_includes.hpp"
#include "../../../projects/attitude_filter/models/star_tracker/star_tracker_model.hpp"
#include "../../../projects/attitude_filter/sim_files/data_bus.hpp"

class StarTrackerSimApp : public SimAppBase<DataBus> {
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

    void step(DataBus& bus, SimControl& sim_ctrl) override {
        star_tracker.inputs.q_j2000_to_body_true = bus.fake_dynamics_outputs.quat;
        star_tracker.inputs.body_rates_true_rps  = bus.fake_dynamics_outputs.body_rates;
        star_tracker.inputs.current_time_sec     = sim_ctrl.public_sim_data().current_sim_time_sec;

        star_tracker.run();

        bus.star_tracker_outputs.measurement_valid            = star_tracker.outputs.star_tracker_measurement_valid;
        bus.star_tracker_outputs.q_j2000_to_star_tracker_meas = star_tracker.outputs.q_j2000_to_star_tracker_meas;
        bus.star_tracker_outputs.measurement_error_rad        = star_tracker.outputs.measurement_error_rad;
        bus.star_tracker_outputs.measurement_time             = star_tracker.outputs.measurement_time;
        bus.star_tracker_outputs.seed                         = star_tracker.outputs.seed;
    }
    
    void teardown(DataBus& bus, SimControl& sim_ctrl) override {
        (void)bus; // Tells compiler that I know these variables are unused
        (void)sim_ctrl;
    }

private:
    StarTrackerModel star_tracker; 
};

#endif