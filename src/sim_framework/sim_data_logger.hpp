#ifndef SIM_DATA_LOGGER_HPP
#define SIM_DATA_LOGGER_HPP

#include <cstddef>
#include <chrono>

#include "../../src/data_logging/logger.hpp"

struct SimMetaData {
    double start_time_sec; 
    double stop_time_sec; 
    double sim_rate_hz; 
    std::size_t num_mc_runs; 
    std::size_t current_mc_run; 
    std::chrono::high_resolution_clock::time_point computer_start_time; 
    std::chrono::high_resolution_clock::time_point computer_stop_time; 
    std::chrono::duration<double> computer_elapsed_seconds; 
    double sim_to_real_time_ratio;
};


class SimDataLogger {
public:
    SimDataLogger(Logger& shared_sim_logger);

    void log_sim_meta_data(const SimMetaData& meta_data) const; 
    void log_current_sim_time(const double& current_sim_time_sec); 

private:
    Logger& logger;
};

#endif
