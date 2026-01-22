#include <gtest/gtest.h>
#include "yaml-cpp/yaml.h"

#include "sim_framework/simulation.hpp"
#include "sim_framework/logging_app_base.hpp"
#include "sim_framework/sim_data_logger.hpp"
#include "sim_framework/sim_control.hpp"
#include "test_sim_app_1.hpp"
#include "test_data_bus.hpp"
#include "test_logger.hpp"
#include "data_logging/read_hdf5_data.hpp"
#include "utilities/file_path_helper_functions.hpp"

struct SimMetaDataRaw {
    double start_time_sec;
    double config_stop_time_sec;
    double actual_stop_time_sec;
    double sim_rate_hz;
    std::size_t num_total_mc_runs;
    std::size_t current_mc_run;
    uint64_t initial_random_seed;
    std::size_t app_count;
    std::size_t logging_app_count;
    double computer_elapsed_time_sec;
    double sim_to_real_time_ratio;
    std::string stop_type;
    std::string stop_reason;
    std::string stop_message;
    std::string computer_start_time;
    std::string computer_stop_time;
};

SimMetaDataRaw get_meta_data(std::string hdf5_file) {
    SimMetaDataRaw meta;

    meta.start_time_sec            = read_hdf5_attribute<double>(hdf5_file,      "/sim", "start_time_sec");
    meta.config_stop_time_sec      = read_hdf5_attribute<double>(hdf5_file,      "/sim", "config_stop_time_sec");
    meta.actual_stop_time_sec      = read_hdf5_attribute<double>(hdf5_file,      "/sim", "actual_stop_time_sec");
    meta.sim_rate_hz               = read_hdf5_attribute<double>(hdf5_file,      "/sim", "sim_rate_hz");
    meta.num_total_mc_runs         = read_hdf5_attribute<uint64_t>(hdf5_file,    "/sim", "num_total_mc_runs");
    meta.current_mc_run            = read_hdf5_attribute<uint64_t>(hdf5_file,    "/sim", "current_mc_run");
    meta.initial_random_seed       = read_hdf5_attribute<uint64_t>(hdf5_file,    "/sim", "initial_random_seed");
    meta.app_count                 = read_hdf5_attribute<uint64_t>(hdf5_file,    "/sim", "app_count");
    meta.logging_app_count         = read_hdf5_attribute<uint64_t>(hdf5_file,    "/sim", "logging_app_count");
    meta.computer_elapsed_time_sec = read_hdf5_attribute<double>(hdf5_file,      "/sim", "computer_elapsed_time_sec");
    meta.sim_to_real_time_ratio    = read_hdf5_attribute<double>(hdf5_file,      "/sim", "sim_to_real_time_ratio");
    meta.stop_type                 = read_hdf5_attribute<std::string>(hdf5_file, "/sim", "stop_type");
    meta.stop_reason               = read_hdf5_attribute<std::string>(hdf5_file, "/sim", "stop_reason");
    meta.stop_message              = read_hdf5_attribute<std::string>(hdf5_file, "/sim", "stop_message");
    meta.computer_start_time       = read_hdf5_attribute<std::string>(hdf5_file, "/sim", "computer_start_time");
    meta.computer_stop_time        = read_hdf5_attribute<std::string>(hdf5_file, "/sim", "computer_stop_time");

    return meta;
}

struct SimCyclicalData {
    std::vector<double>   current_sim_time_sec;
    std::vector<uint64_t> current_sim_time_usec;
    std::vector<uint64_t> sim_step_count;
};

SimCyclicalData get_sim_logged_data(const std::string& hdf5_file) {
    SimCyclicalData data;

    data.current_sim_time_sec  = read_hdf5_dataset<double>(hdf5_file,   "/sim/current_sim_time_sec");
    data.current_sim_time_usec = read_hdf5_dataset<uint64_t>(hdf5_file, "/sim/current_sim_time_usec");
    data.sim_step_count        = read_hdf5_dataset<uint64_t>(hdf5_file, "/sim/sim_step_count");

    return data;
}


TEST(simTests, BasicTest) {
    // Configuration
    const std::string &sim_and_logger_config_path = "../../tests/integrated_sim_and_logging/test_sim_and_logger_config.yaml";

    TestDataBus data_bus;
    Simulation sim(sim_and_logger_config_path, data_bus);

    double app_rate_hz      = 10.0; 
    int app_priority        = 10;
    std::string config_path = "";
    TestSimApp1 test_app_1(app_rate_hz, app_priority, config_path);
    sim.add_app(test_app_1);

    TestLogger logger;
    sim.add_logging_app(logger);
    
    sim.run();


    // Tests
    EXPECT_EQ(test_app_1.app_dt_sec, 1.0/app_rate_hz);
    EXPECT_EQ(test_app_1.priority,   app_priority);


    std::string hdf5_file             = get_absolute_path("tests/integrated_sim_and_logging/test_RUN_00001.hdf5");
    std::vector<int> app_counter_data = read_hdf5_dataset<int>(hdf5_file, "/test_group/counter");
    SimMetaDataRaw  sim_meta_data     = get_meta_data(hdf5_file);
    SimCyclicalData sim_data          = get_sim_logged_data(hdf5_file);

    EXPECT_NEAR(sim_meta_data.actual_stop_time_sec, 15.0, 1e-12);
    EXPECT_EQ(sim_meta_data.app_count, 1);
    EXPECT_TRUE(sim_meta_data.computer_elapsed_time_sec > 0);
    EXPECT_FALSE(sim_meta_data.computer_start_time.empty());
    EXPECT_FALSE(sim_meta_data.computer_stop_time.empty());
    EXPECT_NEAR(sim_meta_data.config_stop_time_sec, sim_meta_data.actual_stop_time_sec, 1e-12);
    EXPECT_EQ(sim_meta_data.current_mc_run, 1);
    EXPECT_EQ(sim_meta_data.initial_random_seed, 0);
    EXPECT_EQ(sim_meta_data.logging_app_count, 1);
    EXPECT_EQ(sim_meta_data.num_total_mc_runs, 1);
    EXPECT_NEAR(sim_meta_data.sim_rate_hz, 10.0, 1e-12);
    EXPECT_TRUE(sim_meta_data.sim_to_real_time_ratio > 0);
    EXPECT_NEAR(sim_meta_data.start_time_sec, 0, 1e-12);
    EXPECT_EQ(sim_meta_data.stop_type, "No Stop (Completed Whole Sim)");
    EXPECT_EQ(sim_meta_data.stop_reason, "Reached Configured Stop Time");
    EXPECT_EQ(sim_meta_data.stop_message, "None Provided");
    



    EXPECT_EQ(sim_data.sim_step_count.size(), app_counter_data.size());

    bool step_increment_matches = true;
    bool time_steps_match       = true;
    double expected_time_sec    = sim_meta_data.start_time_sec;

    for(std::size_t i = 0; i < sim_data.sim_step_count.size(); i++) {
        // Simulation uses zero based step counting so subtracting 1 from `app_counter_Data`
        if (sim_data.sim_step_count[i] != (app_counter_data[i] - 1)) {
            step_increment_matches = false;
        }
    }
    
    for(std::size_t i = 0; i < sim_data.current_sim_time_sec.size(); i++) {
        std::cout << expected_time_sec << "\n";
        std::cout << sim_data.current_sim_time_sec[i] << "\n\n";

        if (std::abs(expected_time_sec - sim_data.current_sim_time_sec[i]) > 1e-6) {
            time_steps_match = false;
        }
        
        expected_time_sec += 1.0 / sim_meta_data.sim_rate_hz;
    }

    EXPECT_TRUE(step_increment_matches);
    EXPECT_TRUE(time_steps_match);

    std::cout << sim_data.current_sim_time_sec.size() << "\n";
    std::cout << sim_data.sim_step_count.size() << "\n";
    
    std::filesystem::remove(hdf5_file); // Delete test file so it isn't commited to repo
}