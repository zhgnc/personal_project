#ifndef TEST_SIM_APP_2_HPP
#define TEST_SIM_APP_2_HPP

#include <memory>
#include <iostream>
#include <string>

#include "../../../sim_framework/generic_apps/sim_app_base.hpp"
#include "../../../tests/integrated_sim_and_logging/common_test_code/test_data_bus.hpp"


class TestSimApp2 : public SimAppBase<TestDataBus> {
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
    };

    void step(TestDataBus& bus, SimControl& sim_ctrl) override {
        count = count + 1;
        seed  = sim_ctrl.get_seed();
        
        bus.app_2_data.counter        = count;
        bus.app_2_data.sim_time_sec   = sim_ctrl.public_sim_data().current_sim_time_sec;
        bus.app_2_data.sim_step_count = sim_ctrl.public_sim_data().sim_step_count;
        bus.app_2_data.sim_dt_sec     = sim_ctrl.public_sim_data().sim_dt_sec;
        bus.app_2_data.sim_rate_hz    = sim_ctrl.public_sim_data().sim_rate_hz;

        if (count >= 10) {
            sim_ctrl.end_sim(StopType::AfterCycle, StopReason::ReachedEndObjective, "Test 2 Stop!");
            sim_ctrl.end_sim(StopType::AfterApp,   StopReason::HardwareFailure,     "Should not throw error and only error data from line above should be captured!");
        } 
    };

    void teardown(TestDataBus& bus, SimControl& sim_ctrl) override {
        (void)bus;

        seed = sim_ctrl.get_seed(); // Seed should have incremented seed 11 times after this line
        sim_ctrl.write_attribute("test_group", "ending_seed", seed);
    };


private:
    int count;  
    uint64_t seed;
};

#endif