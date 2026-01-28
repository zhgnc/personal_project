#ifndef TEST_SIM_APP_1_HPP
#define TEST_SIM_APP_1_HPP

#include <memory>
#include <iostream>
#include <string>

#include "../../../src/sim_framework/sim_app_base.hpp"
#include "../../../tests/integrated_sim_and_logging/common_test_code/test_data_bus.hpp"


class TestSimApp1 : public SimAppBase<TestDataBus> {
public:
    using SimAppBase::SimAppBase;

    void configure_model(const std::string& path_to_config, SimControl& sim_ctrl) override {
        (void)path_to_config; // Tells the compiler I know this varible is unused
        (void)sim_ctrl; 
        
        count = 0;
    };

    void step(TestDataBus& bus, SimControl& sim_ctrl) override {
        count = count + 1;
        bus.app_1_data.counter = count;
    };

    void teardown(TestDataBus& bus, SimControl& sim_ctrl) override {
        (void)bus;
        (void)sim_ctrl;
    };


private:
    int count;  
};

#endif