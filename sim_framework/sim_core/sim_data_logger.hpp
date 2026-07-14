#ifndef SIM_DATA_LOGGER_HPP
#define SIM_DATA_LOGGER_HPP

#include <cstddef>
#include <chrono>
#include <ctime>
#include <mutex>

#include "../../sim_framework/data_logging/logger.hpp"
#include "../../sim_framework/sim_core/sim_control.hpp"

struct SimMetaData {
    double start_time_sec; 
    double config_stop_time_sec;
    double actual_stop_time_sec; 
    StopType stop_type;
    StopReason stop_reason;
    std::string stop_message;
    double sim_rate_hz; 
    std::size_t num_total_mc_runs; 
    std::size_t current_mc_run; 
    uint64_t initial_random_seed;
    std::chrono::high_resolution_clock::time_point computer_start_time; 
    std::chrono::high_resolution_clock::time_point computer_stop_time; 
    std::chrono::duration<double> computer_elapsed_seconds; 
    double sim_to_real_time_ratio;
    std::size_t app_count;
    std::size_t logging_app_count;  
};


class SimDataLogger {
public:
    SimDataLogger(Logger& shared_sim_logger);

    void log_sim_meta_data(const SimMetaData& meta_data) const; 
    void configure_file_with_sim_data(const double&  current_sim_time_sec, 
                                      const uint64_t& current_sim_time_usec, 
                                      const double& sim_rate_hz, 
                                      const uint64_t& sim_step_count);

private:
    Logger& logger;

    // std::localtime writes into a static buffer shared by the whole process, so concurrent
    // SimSingleRun threads calling it at teardown would race on that buffer. This mutex
    // serializes the read of that buffer (via strftime) before releasing it back.
    static std::mutex time_format_mutex;
    static std::string format_time(const std::time_t& time);
};

#endif
