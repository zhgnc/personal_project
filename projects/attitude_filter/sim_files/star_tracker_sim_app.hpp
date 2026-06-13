#ifndef STAR_TRACKER_SIM_APP_HPP
#define STAR_TRACKER_SIM_APP_HPP

#include <memory>
#include <iostream>
#include <string>

#include "../../../sim_framework/sim_includes.hpp"
#include "../../../projects/attitude_filter/models/star_tracker/star_tracker_model.hpp"
#include "../../../projects/attitude_filter/sim_files/data_bus.hpp"

class StarTrackerSimApp : public SimAppBase<DataBus> {
public:
    using SimAppBase::SimAppBase;

    void configure_model(const std::string& path_to_config, SimControl& sim_ctrl) override {
        uint64_t seed = sim_ctrl.get_seed();  
        star_tracker  = StarTrackerModel(path_to_config, seed);
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