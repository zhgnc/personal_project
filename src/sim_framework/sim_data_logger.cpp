#include "../../src/sim_framework/sim_data_logger.hpp"

SimDataLogger::SimDataLogger(Logger& shared_sim_logger)
    : logger(shared_sim_logger)  // Reference members cannot be assigned in the constructor body
{};

// This function must be called after the output hdf5 file is created
void SimDataLogger::configure_file_with_sim_data(const double& current_sim_time_sec, 
                                                 const uint64_t& current_sim_time_usec, 
                                                 const double& sim_rate_hz, 
                                                 const uint64_t& sim_step_count) 
{
    logger.add_group("/sim");
    logger.add_dataset<double>("current_sim_time_sec", "/sim", current_sim_time_sec, sim_rate_hz);
    logger.add_dataset<uint64_t>("current_sim_time_usec", "/sim", current_sim_time_usec, sim_rate_hz);
    logger.add_dataset<uint64_t>("sim_step_count", "/sim", sim_step_count, sim_rate_hz);
}

void SimDataLogger::log_sim_meta_data(const SimMetaData& meta_data) const {
    logger.write_attribute("/sim", "start_time_sec",         meta_data.start_time_sec);
    logger.write_attribute("/sim", "config_stop_time_sec",   meta_data.config_stop_time_sec);
    logger.write_attribute("/sim", "actual_stop_time_sec",   meta_data.actual_stop_time_sec);
    
    std::string stop_type = SimulationControl::stop_type_to_string(meta_data.stop_type);
    std::string stop_reason = SimulationControl::stop_reason_to_string(meta_data.stop_reason);

    logger.write_attribute("/sim", "stop_type",            stop_type);
    logger.write_attribute("/sim", "stop_reason",          stop_reason);
    logger.write_attribute("/sim", "stop_message",         meta_data.stop_message);
    logger.write_attribute("/sim", "sim_rate_hz",          meta_data.sim_rate_hz);
    logger.write_attribute("/sim", "num_total_mc_runs",    meta_data.num_total_mc_runs);
    logger.write_attribute("/sim", "current_mc_run",       meta_data.current_mc_run);
    logger.write_attribute("/sim", "initial_random_seed",  meta_data.initial_random_seed);

    std::time_t start_sec_since_epoch = std::chrono::system_clock::to_time_t(meta_data.computer_start_time);
    std::time_t stop_sec_since_epoch  = std::chrono::system_clock::to_time_t(meta_data.computer_stop_time);

    logger.write_attribute("/sim", "computer_start_time",       std::ctime(&start_sec_since_epoch));
    logger.write_attribute("/sim", "computer_stop_time",        std::ctime(&stop_sec_since_epoch));
    logger.write_attribute("/sim", "computer_elapsed_time_sec", meta_data.computer_elapsed_seconds.count());
    logger.write_attribute("/sim", "sim_to_real_time_ratio",    meta_data.sim_to_real_time_ratio);
    logger.write_attribute("/sim", "app_count",                 meta_data.app_count);
    logger.write_attribute("/sim", "logging_app_count",         meta_data.logging_app_count);
}