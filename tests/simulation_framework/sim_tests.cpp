#include <gtest/gtest.h>
#include "yaml-cpp/yaml.h"

#include "sim_framework/simulation.hpp"
#include "sim_framework/logging_app_base.hpp"
#include "test_sim_app_1.hpp"
#include "test_data_bus.hpp"
#include "test_logger.hpp"

TEST(simTests, BasicTest) {
    const std::string &sim_config_path     = "../../tests/simulation_framework/test_sim_config.yaml";
    const std::string &logging_config_path = "../../tests/simulation_framework/test_logging_config.yaml";

    TestDataBus data_bus;
    Simulation sim(sim_config_path, data_bus);

    double app_rate_hz      = 1.0; 
    int app_priority        = 10;
    std::string config_path = "";
    std::shared_ptr<SimAppBase<TestDataBus>> test_app_1 = std::make_shared<TestSimApp1>(app_rate_hz, app_priority, config_path);
    sim.add_app(test_app_1);

    std::shared_ptr<TestLogger> logging_app = std::make_shared<TestLogger>(logging_config_path);
    sim.add_logger(logging_app);

    sim.run();

    EXPECT_EQ(test_app_1->app_dt_sec, app_rate_hz);
    EXPECT_EQ(test_app_1->priority,   app_priority);
    EXPECT_EQ(1,2);
}