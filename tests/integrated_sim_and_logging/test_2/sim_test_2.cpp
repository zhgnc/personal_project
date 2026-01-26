#include <gtest/gtest.h>
#include "yaml-cpp/yaml.h"

#include "sim_framework/simulation.hpp"
#include "sim_framework/logging_app_base.hpp"
#include "sim_framework/sim_data_logger.hpp"
#include "sim_framework/sim_control.hpp"
#include "integrated_sim_and_logging/common_test_code/test_sim_app_1.hpp"
#include "integrated_sim_and_logging/common_test_code/test_sim_app_2.hpp"
#include "integrated_sim_and_logging/common_test_code/test_data_bus.hpp"
#include "integrated_sim_and_logging/test_2/test_logger_2.hpp"
#include "data_logging/read_hdf5_data.hpp"
#include "utilities/file_path_helper_functions.hpp"
#include "integrated_sim_and_logging/common_test_code/helper_functions.hpp"

TEST(simTest2, TwoAppsWithStopTest) {
    // Configuration
    double tol = 1e-10;
    const std::string &sim_and_logger_config_path = "../../tests/integrated_sim_and_logging/test_2/test_2_sim_and_logger_config.yaml";

    TestDataBus data_bus;
    Simulation sim(sim_and_logger_config_path, data_bus);

    double app_1_rate_hz    = 10.0; 
    int app_1_priority      = 1;
    std::string config_path = "";
    TestSimApp1 test_app_1("test_app_1", app_1_rate_hz, app_1_priority, config_path);
    sim.add_app(test_app_1);

    double app_2_rate_hz    = 0.5; 
    int app_2_priority      = 2;
    TestSimApp2 test_app_2("test_app_2", app_2_rate_hz, app_2_priority, config_path);
    sim.add_app(test_app_2);

    TestLogger2 logger;
    sim.add_logging_app(logger);
    
    sim.run();


    // Tests (Only checking meta data that is different than test 1)
    EXPECT_EQ(test_app_1.dt_sec(), 1.0/app_1_rate_hz);
    EXPECT_EQ(test_app_1.priority(), app_1_priority);
    EXPECT_EQ(test_app_2.dt_sec(), 1.0/app_2_rate_hz);
    EXPECT_EQ(test_app_2.priority(), app_2_priority);

    std::string hdf5_file               = get_absolute_path("tests/integrated_sim_and_logging/test_2/test_RUN_00001.hdf5");
    std::vector<int> app_2_counter_data = read_hdf5_dataset<int>(hdf5_file, "/test_group/counter_2");
    SimMetaDataRaw  sim_meta_data       = get_meta_data(hdf5_file);
    SimCyclicalData sim_data            = get_sim_logged_data(hdf5_file);
    SimuRunYamlConfig config_data       = load_simulation_run_config(sim_and_logger_config_path);

    EXPECT_NEAR(sim_meta_data.actual_stop_time_sec, 20, tol);
    EXPECT_EQ(sim_meta_data.app_count, 2);
    EXPECT_NEAR(sim_meta_data.sim_rate_hz, config_data.simulation_rate_hz, tol);
    EXPECT_NEAR(sim_meta_data.start_time_sec, config_data.sim_start_time_sec, tol);
    EXPECT_EQ(sim_meta_data.stop_type, "After This Cycle");
    EXPECT_EQ(sim_meta_data.stop_reason, "Reached End Objective");
    EXPECT_EQ(sim_meta_data.stop_message, "Test 2 Stop!");

    bool step_increment_matches_app2 = true;
    double sec_to_usec               = 1e6;
    uint64_t expected_time_usec      = static_cast<uint64_t>(sim_meta_data.start_time_sec * sec_to_usec);
    int sim_to_app2_ratio            = static_cast<int>(sim_meta_data.sim_rate_hz / app_2_rate_hz);

    for (std::size_t i = 0; i < app_2_counter_data.size(); i++) {
        std::size_t sim_step = i * sim_to_app2_ratio;

        // Simulation uses zero based step counting so subtracting 1 from `app_counter_Data`
        if (sim_data.sim_step_count[sim_step] / sim_to_app2_ratio != app_2_counter_data[i] - 1) {
            step_increment_matches_app2 = false;
        }
    }
    
    EXPECT_TRUE(step_increment_matches_app2);
    
    std::filesystem::remove(hdf5_file); // Delete test file so it isn't commited to repo
}