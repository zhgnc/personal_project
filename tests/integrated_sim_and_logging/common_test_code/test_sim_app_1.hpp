#ifndef TEST_SIM_APP_1_HPP
#define TEST_SIM_APP_1_HPP

#include <memory>
#include <iostream>
#include <string>

#include "../../../sim_framework/generic_apps/sim_app_base.hpp"


class TestSimApp1 : public SimAppBase {
public:
    using SimAppBase::SimAppBase;

    void configure_model(const std::string& path_to_config, SimControl& sim_ctrl) override {
        (void)path_to_config; // Tells the compiler I know this varible is unused
        (void)sim_ctrl;

        count = 0;
    };

    void declare_io(IoRegistry& io) override {
        io.tlm_req("counter", count);
    };

    void step(SimControl& sim_ctrl) override {
        (void)sim_ctrl;

        count = count + 1;
    };

    void teardown(SimControl& sim_ctrl) override {
        (void)sim_ctrl;
    };


private:
    int count;
};

#endif
