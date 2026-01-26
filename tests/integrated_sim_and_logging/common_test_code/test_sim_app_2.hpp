#ifndef TEST_SIM_APP_2_HPP
#define TEST_SIM_APP_2_HPP

#include <memory>
#include <iostream>
#include <string>

#include "../../../src/sim_framework/sim_app_base.hpp"
#include "../../../tests/integrated_sim_and_logging/common_test_code/test_data_bus.hpp"


class TestSimApp2 : public SimAppBase<TestDataBus> {
public:
    using SimAppBase::SimAppBase;

    void configure_model(const std::string& path_to_config, SimControl& sim_ctrl) override {
        (void)path_to_config; // Tells the compiler I know this varible is unused
        (void)sim_ctrl; 
        
        count = 0;

        // Calling get_next seed three times to verify it is incrementing correctly
        seed = sim_ctrl.get_next_seed();
        seed = sim_ctrl.get_next_seed();
        seed = sim_ctrl.get_next_seed();
        // sim_ctrl.write_attibute<uint64_t>("test_group/", "app2_seed", data_bus.app_2_data.seed);
        
        sim_ctrl.public_sim_data().current_sim_time_sec;
        sim_ctrl.public_sim_data().current_sim_time_sec;
        sim_ctrl.public_sim_data().current_sim_time_sec;
        sim_ctrl.public_sim_data().current_sim_time_sec;
    };

    void step(TestDataBus& bus, SimControl& sim_ctrl) override {
        count = count + 1;
        bus.app_2_data.counter = count;

        if (count > 10) {
            sim_ctrl.end_sim(StopType::AfterCycle, StopReason::ReachedEndObjective, "Test 2 Stop!");
            sim_ctrl.end_sim(StopType::AfterApp,   StopReason::HardwareFailure,     "Should not throw error and only error data from line above should be captured!");
        } 
    };

    void teardown(TestDataBus& bus, SimControl& sim_ctrl) override {
        (void)bus;
        (void)sim_ctrl;
    };


private:
    int count;  
    uint64_t seed;
};

#endif