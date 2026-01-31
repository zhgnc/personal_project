#include "helper_functions.hpp"

SimMetaDataRaw get_meta_data(const std::string& hdf5_file) {
    SimMetaDataRaw meta{};
    meta.start_time_sec            = read_hdf5_attribute<double>(hdf5_file, "/sim", "start_time_sec");
    meta.config_stop_time_sec      = read_hdf5_attribute<double>(hdf5_file, "/sim", "config_stop_time_sec");
    meta.actual_stop_time_sec      = read_hdf5_attribute<double>(hdf5_file, "/sim", "actual_stop_time_sec");
    meta.sim_rate_hz               = read_hdf5_attribute<double>(hdf5_file, "/sim", "sim_rate_hz");
    meta.num_total_mc_runs         = read_hdf5_attribute<uint64_t>(hdf5_file, "/sim", "num_total_mc_runs");
    meta.current_mc_run            = read_hdf5_attribute<uint64_t>(hdf5_file, "/sim", "current_mc_run");
    meta.initial_random_seed       = read_hdf5_attribute<uint64_t>(hdf5_file, "/sim", "initial_random_seed");
    meta.app_count                 = read_hdf5_attribute<uint64_t>(hdf5_file, "/sim", "app_count");
    meta.logging_app_count         = read_hdf5_attribute<uint64_t>(hdf5_file, "/sim", "logging_app_count");
    meta.computer_elapsed_time_sec = read_hdf5_attribute<double>(hdf5_file, "/sim", "computer_elapsed_time_sec");
    meta.sim_to_real_time_ratio    = read_hdf5_attribute<double>(hdf5_file, "/sim", "sim_to_real_time_ratio");
    meta.stop_type                 = read_hdf5_attribute<std::string>(hdf5_file, "/sim", "stop_type");
    meta.stop_reason               = read_hdf5_attribute<std::string>(hdf5_file, "/sim", "stop_reason");
    meta.stop_message              = read_hdf5_attribute<std::string>(hdf5_file, "/sim", "stop_message");
    meta.computer_start_time       = read_hdf5_attribute<std::string>(hdf5_file, "/sim", "computer_start_time");
    meta.computer_stop_time        = read_hdf5_attribute<std::string>(hdf5_file, "/sim", "computer_stop_time");
    return meta;
}

SimCyclicalData get_sim_logged_data(const std::string& hdf5_file) {
    SimCyclicalData data{};
    data.current_sim_time_sec  = read_hdf5_dataset<double>(hdf5_file, "/sim/current_sim_time_sec");
    data.current_sim_time_usec = read_hdf5_dataset<uint64_t>(hdf5_file, "/sim/current_sim_time_usec");
    data.sim_step_count        = read_hdf5_dataset<uint64_t>(hdf5_file, "/sim/sim_step_count");
    return data;
}

SimuRunYamlConfig load_simulation_run_config(const std::string& yaml_path) {
    YAML::Node root = load_yaml_file(yaml_path);
    SimuRunYamlConfig config{};
    config.sim_start_time_sec                   = get_yaml_value<double>(root, "sim_start_time_sec");
    config.sim_stop_time_sec                    = get_yaml_value<double>(root, "sim_stop_time_sec");
    config.simulation_rate_hz                   = get_yaml_value<double>(root, "simulation_rate_hz");
    config.number_of_monte_carlo_runs           = get_yaml_value<std::size_t>(root, "number_of_monte_carlo_runs");
    config.initial_random_seed                  = get_yaml_value<uint64_t>(root, "initial_random_seed");
    config.print_hdf5_file_format               = get_yaml_value<bool>(root, "print_hdf5_file_format");
    config.print_hdf5_attributes_in_file_format = get_yaml_value<bool>(root, "print_hdf5_attributes_in_file_format");
    config.logging_filename_prefix              = get_yaml_value<std::string>(root, "logging_filename_prefix");
    config.logging_file_save_directory          = get_yaml_value<std::string>(root, "logging_file_save_directory");
    config.logging_rate_A_hz                    = get_yaml_value<double>(root, "logging_rate_A_hz");
    config.logging_rate_B_hz                    = get_yaml_value<double>(root, "logging_rate_B_hz");
    config.logging_rate_C_hz                    = get_yaml_value<double>(root, "logging_rate_C_hz");
    config.logging_rate_D_hz                    = get_yaml_value<double>(root, "logging_rate_D_hz");
    config.logging_rate_E_hz                    = get_yaml_value<double>(root, "logging_rate_E_hz");
    return config;
}