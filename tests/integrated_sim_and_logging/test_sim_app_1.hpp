#ifndef TEST_SIM_APP_1_HPP
#define TEST_SIM_APP_1_HPP

#include <memory>
#include <iostream>
#include <string>

#include "../../../src/sim_framework/sim_app_base.hpp"
#include "../../../tests/integrated_sim_and_logging/test_data_bus.hpp"


class TestSimApp1 : public SimAppBase<TestDataBus> {
public:
    using SimAppBase::SimAppBase;

    void configure_model(const std::string& /*not_used_config_path*/) override {}

    void step(TestDataBus& bus, SimulationControl& sim_ctrl) override {
        count = count + 1;
        bus.app_1_data.counter = count;
    };


private:
    // Sim start time is 0 and the count will be 0 on first sim cycle if count is -1 at initialization (-1 + 1 = 0)
    int count = -1;  
};

#endif