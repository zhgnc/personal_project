#ifndef FAKE_DYNAMICS_SIM_APP_HPP
#define FAKE_DYNAMICS_SIM_APP_HPP

#include "../../../sim_framework/sim_includes.hpp"
#include "data_bus.hpp"
#include "utilities/yaml_utilities.hpp"
#include "math/math.hpp"

#include <cmath>

class FakeDynamicsSimApp : public SimAppBase<DataBus> {
public:
    using SimAppBase::SimAppBase;

    void configure_model(const std::string& path_to_config, SimControl& sim_ctrl) override {
        (void)sim_ctrl;

        YAML::Node config_data = load_yaml_file(path_to_config);

        q_attitude           = get_yaml_value<std::array<double,4>>(config_data, "initial_quaternion");
        q_attitude           = q_attitude.normalize(); 
        rot_vec_attitude     = to_rot_vec(q_attitude);

        x_axis_period_s      = get_yaml_value<double>(config_data, "x_axis_period_s");
        x_axis_amplitude_rps = get_yaml_value<double>(config_data, "x_axis_amplitude_dps") * deg2rad;
        x_axis_shift_rad     = get_yaml_value<double>(config_data, "x_axis_shift_deg") * deg2rad;
        
        y_axis_period_s      = get_yaml_value<double>(config_data, "y_axis_period_s");
        y_axis_amplitude_rps = get_yaml_value<double>(config_data, "y_axis_amplitude_dps") * deg2rad;
        y_axis_shift_rad     = get_yaml_value<double>(config_data, "y_axis_shift_deg") * deg2rad;
        
        z_axis_period_s      = get_yaml_value<double>(config_data, "z_axis_period_s");
        z_axis_amplitude_rps = get_yaml_value<double>(config_data, "z_axis_amplitude_dps") * deg2rad;
        z_axis_shift_rad     = get_yaml_value<double>(config_data, "z_axis_shift_deg") * deg2rad;

        if (x_axis_period_s <= 0.0 || y_axis_period_s <= 0.0 || z_axis_period_s <= 0.0) {
            throw std::runtime_error("[fake_dynamics_sim_app.hpp] All axis periods must be > 0 (check fake_dynamics_config.yaml)");
        }
    }

    void step(DataBus& bus, SimControl& sim_ctrl) override {
        t  = sim_ctrl.public_sim_data().current_sim_time_sec;
        dt = sim_ctrl.public_sim_data().sim_dt_sec; 
        

        body_rates(0) = x_axis_amplitude_rps * sin((2*M_PI/x_axis_period_s) * t + x_axis_shift_rad);
        body_rates(1) = y_axis_amplitude_rps * sin((2*M_PI/y_axis_period_s) * t + y_axis_shift_rad);
        body_rates(2) = z_axis_amplitude_rps * sin((2*M_PI/z_axis_period_s) * t + z_axis_shift_rad);

        rot_vec_attitude = rot_vec_attitude + body_rates * dt;
        q_attitude       = to_quat(rot_vec_attitude).normalize();
    
        bus.fake_dynamics_outputs.quat             = q_attitude;
        bus.fake_dynamics_outputs.body_rates       = body_rates;
        bus.fake_dynamics_outputs.rot_vec_attitude = rot_vec_attitude;
    }
    
    void teardown(DataBus& bus, SimControl& sim_ctrl) override {
        (void)bus; // Tells compiler that I know these variables are unused
        (void)sim_ctrl;
    }

private:
    double t;
    double dt;

    vector<double, 3> body_rates;
    rot_vec<double> rot_vec_attitude;
    quat<double> q_attitude;
    

    double x_axis_period_s;
    double x_axis_amplitude_rps;
    double x_axis_shift_rad;

    double y_axis_period_s;
    double y_axis_amplitude_rps;
    double y_axis_shift_rad;

    double z_axis_period_s;
    double z_axis_amplitude_rps;
    double z_axis_shift_rad;

    double deg2rad = M_PI/180.0;
};

#endif