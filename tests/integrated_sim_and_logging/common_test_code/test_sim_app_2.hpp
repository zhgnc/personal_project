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

    void configure_model(const std::string& path_to_config, SimulationControl& sim_ctrl) override {
        (void)path_to_config; // Tells the compiler I know this varible is unused
        (void)sim_ctrl; 
        
        count = 0;
    };

    void step(TestDataBus& bus, SimulationControl& sim_ctrl) override {
        count = count + 1;
        bus.app_1_data.counter = count;

        if (count > 10) {
            sim_ctrl.end_sim_after_cycle(StopReason::ReachedEndObjective, "Test 2 Stop!");
        } 
    };

    void teardown(TestDataBus& bus, SimulationControl& sim_ctrl) override {
        (void)bus;
        (void)sim_ctrl;
    };


private:
    int count;  
};

#endif