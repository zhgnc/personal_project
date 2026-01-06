#include <gtest/gtest.h>
#include "yaml-cpp/yaml.h"

#include "sim_framework/simulation.hpp"
#include "sim_framework/logging_app_base.hpp"
#include "test_sim_app_1.hpp"
#include "test_data_bus.hpp"
#include "test_logger.hpp"
#include "data_logging/read_hdf5_data.hpp"
#include "utilities/file_path_helper_functions.hpp"

TEST(simTests, BasicTest) {
    int sim_length  = 15;
    int sim_rate_hz = 10;

    const std::string &sim_config_path     = "../../tests/integrated_sim_and_logging/test_sim_config.yaml";
    const std::string &logging_config_path = "../../tests/integrated_sim_and_logging/test_logging_config.yaml";

    TestDataBus data_bus;
    Simulation sim(sim_config_path, data_bus);

    double app_rate_hz      = 10.0; 
    int app_priority        = 10;
    std::string config_path = "";
    std::shared_ptr<SimAppBase<TestDataBus>> test_app_1 = std::make_shared<TestSimApp1>(app_rate_hz, app_priority, config_path);
    sim.add_app(test_app_1);

    std::shared_ptr<TestLogger> logging_app = std::make_shared<TestLogger>(logging_config_path);
    sim.add_logger(logging_app);

    sim.run();



    std::string hdf5_file         = get_absolute_path("tests/integrated_sim_and_logging/test_RUN_00000.hdf5");
    std::vector<int> counter_data = read_hdf5_dataset<int>(hdf5_file, "/test_group/counter");

    EXPECT_EQ(test_app_1->app_dt_sec, 1/app_rate_hz);
    EXPECT_EQ(test_app_1->priority,   app_priority);

    int total_step = sim_length * sim_rate_hz;

    for(std::size_t i = 0; i < total_step; i++) {
        EXPECT_EQ(counter_data[i], i);
    }

    std::filesystem::remove(hdf5_file); // Delete test file so it isn't commited to repo
}