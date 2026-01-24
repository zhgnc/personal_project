#ifndef HELPER_FUNCTIONS_HPP
#define HELPER_FUNCTIONS_HPP

#include <string>
#include <vector>
#include <cstdint>

#include "data_logging/read_hdf5_data.hpp"
#include "utilities/yaml_utilities.hpp"
#include "data_logging/logger.hpp"


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

struct SimCyclicalData {
    std::vector<double>   current_sim_time_sec;
    std::vector<uint64_t> current_sim_time_usec;
    std::vector<uint64_t> sim_step_count;
};

struct SimuRunYamlConfig {
    double sim_start_time_sec;
    double sim_stop_time_sec;
    double simulation_rate_hz;
    std::size_t number_of_monte_carlo_runs;
    uint64_t initial_random_seed;
    bool print_hdf5_file_format;
    bool print_hdf5_attributes_in_file_format;
    std::string logging_filename_prefix;
    std::string logging_file_save_directory;
    double logging_rate_A_hz;
    double logging_rate_B_hz;
    double logging_rate_C_hz;
    double logging_rate_D_hz;
    double logging_rate_E_hz;
};

SimMetaDataRaw get_meta_data(const std::string& hdf5_file);
SimCyclicalData get_sim_logged_data(const std::string& hdf5_file);
SimuRunYamlConfig load_simulation_run_config(const std::string& yaml_path);

#endif