#ifndef SIM_SINGLE_RUN_HPP
#define SIM_SINGLE_RUN_HPP

#include "sim_app_base.hpp"
#include "logging_app_base.hpp"
#include "sim_data_logger.hpp"
#include "sim_control.hpp"
#include "../data_logging/logger.hpp"

#include <vector>
#include <memory>
#include <string>
#include <cstdint>
#include <chrono>


struct SimSingleRunConfig {
    double start_time_sec;
    double config_stop_time_sec;         
    double sim_rate_hz;           
    std::string base_file_name;  
    std::string data_output_directory;
    AppLoggingRates logging_rates;
    std::size_t current_mc_run;
    std::size_t random_seed;
};


template<typename DataBusType>
class SimSingleRun : public SimulationControl {
public:
    SimSingleRun(std::vector<std::shared_ptr<SimAppBase<DataBusType>>>&& apps,
                 std::vector<std::shared_ptr<LoggingAppBase<DataBusType>>>&& logging_apps,
                 DataBusType& bus,
                 const SimSingleRunConfig& config);

    void run();

private:
    void setup();
    void step();
    void teardown();

    DataBusType& data_bus;
    Logger logger;

    std::vector<std::shared_ptr<SimAppBase<DataBusType>>> app_list;
    std::vector<std::shared_ptr<LoggingAppBase<DataBusType>>> logging_apps;
    std::unique_ptr<SimDataLogger> sim_data_logger;

    SimSingleRunConfig config;
    bool end_sim;
    StopReason stop_reason;;
    double actual_stop_time_sec;
    uint64_t sim_dt_usec;
    uint64_t current_sim_time_usec;
    uint64_t config_stop_time_usec;
    double current_sim_time_sec;

    double sec2usec = 1e6;
};

#include "sim_single_run.tpp"

#endif
