#ifndef TEST_SIM_APP_2_HPP
#define TEST_SIM_APP_2_HPP

#include <memory>
#include <iostream>
#include <string>

#include "../../../sim_framework/generic_apps/sim_app_base.hpp"


class TestSimApp2 : public SimAppBase {
public:
    using SimAppBase::SimAppBase;

    void configure_model(const std::string& path_to_config, SimControl& sim_ctrl) override {
        (void)path_to_config; // Tells the compiler I know this varible is unused

        count = 0;
        seed  = sim_ctrl.get_seed(); // Should return seed equal to config which is zero

        sim_ctrl.write_attribute("test_group", "init_seed",                 seed);
        sim_ctrl.write_attribute("test_group", "init_current_sim_time_sec", sim_ctrl.public_sim_data().current_sim_time_sec);
        sim_ctrl.write_attribute("test_group", "init_sim_dt_sec",           sim_ctrl.public_sim_data().sim_dt_sec);
        sim_ctrl.write_attribute("test_group", "init_sim_rate_hz",          sim_ctrl.public_sim_data().sim_rate_hz);
        sim_ctrl.write_attribute("test_group", "init_sim_step_count",       sim_ctrl.public_sim_data().sim_step_count);

        // Previously written by the (now removed) test logging app; verifies
        // attribute writes are visible in the file alongside recorded ports
        sim_ctrl.write_attribute("test_group", "test_attribute_write_in_logger", static_cast<uint64_t>(12345));
    };

    void declare_io(IoRegistry& io) override {
        io.tlm_req("counter",        count);
        io.tlm_req("sim_time_sec",   sim_time_sec, Units::Seconds);
        io.tlm_req("sim_step_count", sim_step_count);
        io.tlm_req("sim_rate_hz",    sim_rate_hz, Units::Hertz);
        io.tlm_req("sim_dt_sec",     sim_dt_sec, Units::Seconds);
    };

    void step(SimControl& sim_ctrl) override {
        count = count + 1;
        seed  = sim_ctrl.get_seed();

        sim_time_sec   = sim_ctrl.public_sim_data().current_sim_time_sec;
        sim_step_count = sim_ctrl.public_sim_data().sim_step_count;
        sim_dt_sec     = sim_ctrl.public_sim_data().sim_dt_sec;
        sim_rate_hz    = sim_ctrl.public_sim_data().sim_rate_hz;

        if (count >= 10) {
            sim_ctrl.end_sim(StopType::AfterCycle, StopReason::ReachedEndObjective, "Test 2 Stop!");
            sim_ctrl.end_sim(StopType::AfterApp,   StopReason::HardwareFailure,     "Should not throw error and only error data from line above should be captured!");
        }
    };

    void teardown(SimControl& sim_ctrl) override {
        seed = sim_ctrl.get_seed(); // Seed should have incremented seed 11 times after this line
        sim_ctrl.write_attribute("test_group", "ending_seed", seed);
    };


private:
    int count;
    uint64_t seed;

    double sim_time_sec     = 0;
    uint64_t sim_step_count = 0;
    double sim_dt_sec       = 0;
    double sim_rate_hz      = 0;
};

#endif
