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
    TestSimApp1 test_app_1(app_1_rate_hz, app_1_priority, config_path);
    sim.add_app(test_app_1);

    double app_2_rate_hz    = 0.5; 
    int app_2_priority      = 2;
    TestSimApp2 test_app_2(app_2_rate_hz, app_2_priority, config_path);
    sim.add_app(test_app_2);

    TestLogger2 logger;
    sim.add_logging_app(logger);
    
    sim.run();

    EXPECT_EQ(1,2);


    // Tests
    EXPECT_EQ(test_app_1.app_dt_sec, 1.0/app_1_rate_hz);
    EXPECT_EQ(test_app_1.priority,   app_1_priority);
    EXPECT_EQ(test_app_2.app_dt_sec, 1.0/app_2_rate_hz);
    EXPECT_EQ(test_app_2.priority,   app_2_priority);

    std::string hdf5_file               = get_absolute_path("tests/integrated_sim_and_logging/test_2/test_RUN_00001.hdf5");
    std::vector<int> app_1_counter_data = read_hdf5_dataset<int>(hdf5_file, "/test_group/counter_1");
    std::vector<int> app_2_counter_data = read_hdf5_dataset<int>(hdf5_file, "/test_group/counter_2");
    SimMetaDataRaw  sim_meta_data       = get_meta_data(hdf5_file);
    SimCyclicalData sim_data            = get_sim_logged_data(hdf5_file);
    SimuRunYamlConfig config_data       = load_simulation_run_config(sim_and_logger_config_path);

    EXPECT_NEAR(sim_meta_data.actual_stop_time_sec, 20, tol);
    EXPECT_EQ(sim_meta_data.app_count, 2);
    EXPECT_TRUE(sim_meta_data.computer_elapsed_time_sec > 0);
    EXPECT_FALSE(sim_meta_data.computer_start_time.empty());
    EXPECT_FALSE(sim_meta_data.computer_stop_time.empty());
    EXPECT_NEAR(sim_meta_data.config_stop_time_sec, config_data.sim_stop_time_sec, tol);
    EXPECT_EQ(sim_meta_data.current_mc_run, 1);
    EXPECT_EQ(sim_meta_data.initial_random_seed, config_data.initial_random_seed);
    EXPECT_EQ(sim_meta_data.logging_app_count, 1);
    EXPECT_EQ(sim_meta_data.num_total_mc_runs, config_data.number_of_monte_carlo_runs);
    EXPECT_NEAR(sim_meta_data.sim_rate_hz, config_data.simulation_rate_hz, tol);
    EXPECT_TRUE(sim_meta_data.sim_to_real_time_ratio > 0);
    EXPECT_NEAR(sim_meta_data.start_time_sec, config_data.sim_start_time_sec, tol);
    EXPECT_EQ(sim_meta_data.stop_type, "After This Cycle");
    EXPECT_EQ(sim_meta_data.stop_reason, "Reached End Objective");
    EXPECT_EQ(sim_meta_data.stop_message, "Test 2 Stop!");

    EXPECT_EQ(sim_data.sim_step_count.size(), app_1_counter_data.size());

    bool step_increment_matches = true;
    bool sec_time_steps_match   = true;
    bool usec_time_steps_match  = true;
    double sec_to_usec          = 1e6;
    double expected_time_sec    = sim_meta_data.start_time_sec;
    uint64_t expected_time_usec = static_cast<uint64_t>(sim_meta_data.start_time_sec * sec_to_usec);

    for(std::size_t i = 0; i < sim_data.sim_step_count.size(); i++) {
        // Simulation uses zero based step counting so subtracting 1 from `app_1_counter_data`
        std::cout << sim_data.sim_step_count[i] << "      " << app_1_counter_data[i] - 1 << "\n";
        
        if (sim_data.sim_step_count[i] != (app_1_counter_data[i] - 1)) {
            step_increment_matches = false;
        }
    }
    
    for(std::size_t i = 0; i < sim_data.current_sim_time_sec.size(); i++) {
        if (std::abs(expected_time_sec - sim_data.current_sim_time_sec[i]) > tol) {
            sec_time_steps_match = false;
        }

        if (expected_time_usec != sim_data.current_sim_time_usec[i]) {
            usec_time_steps_match = false;
        }
        
        expected_time_sec  += 1.0 / sim_meta_data.sim_rate_hz;
        expected_time_usec += static_cast<uint64_t>(sec_to_usec / sim_meta_data.sim_rate_hz);
    }

    EXPECT_TRUE(step_increment_matches);
    EXPECT_TRUE(sec_time_steps_match);
    EXPECT_TRUE(usec_time_steps_match);
    
    std::filesystem::remove(hdf5_file); // Delete test file so it isn't commited to repo
}