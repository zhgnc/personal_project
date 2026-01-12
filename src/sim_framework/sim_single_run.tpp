#include "sim_single_run.hpp"

template<typename DataBusType>
SimSingleRun<DataBusType>::SimSingleRun(std::vector<std::shared_ptr<SimAppBase<DataBusType>>>&& apps,
                                        std::vector<std::shared_ptr<LoggingAppBase<DataBusType>>>&& log_apps,
                                        DataBusType& bus,
                                        const SimSingleRunConfig& single_run_config)
    : data_bus(bus), 
      app_list(std::move(apps)), 
      logging_apps(std::move(log_apps)), 
      config(single_run_config)
{
    current_sim_time_usec = static_cast<uint64_t>(config.start_time_sec * sec2usec);
    config_stop_time_usec = static_cast<uint64_t>(config.config_stop_time_sec * sec2usec);
    current_sim_time_sec  = config.start_time_sec;
    
    sim_dt_usec          = static_cast<uint64_t>(sec2usec * (1.0 / config.sim_rate_hz));
    end_sim              = false;
    stop_reason          = StopReason::ReachedConfiguredStopTime;
    actual_stop_time_sec = config.config_stop_time_sec;
}

template<typename DataBusType>
void SimSingleRun<DataBusType>::run() {
    setup();

    while (current_sim_time_usec <= config_stop_time_usec && end_sim == false) {
        step();
    }

    teardown();
}

template<typename DataBusType>
void SimSingleRun<DataBusType>::setup() {
    std::string new_file_name = std::format("{}_RUN_{:05}.hdf5", config.base_file_name, config.current_mc_run);
    std::string full_path     = config.data_output_directory + "/" + new_file_name;

    logger.create_file(full_path);

    for (std::shared_ptr<LoggingAppBase<DataBusType>>& logging_app : logging_apps) {
        logging_app->config_hdf5_with_app_data(logger, data_bus, config.logging_rates);
    }

    sim_data_logger = std::make_unique<SimDataLogger>(logger);
    sim_data_logger->configure_file_with_sim_data(current_sim_time_sec,
                                                  current_sim_time_usec,
                                                  config.sim_rate_hz);

    computer_start_time = std::chrono::high_resolution_clock::now();
}

template<typename DataBusType>
void SimSingleRun<DataBusType>::step() {
    SimulationControl& sim_ctrl = *this;

    for (std::shared_ptr<SimAppBase<DataBusType>>& app : app_list) {
        app->check_step(current_sim_time_usec, data_bus, sim_ctrl);
    }

    logger.log_data(current_sim_time_usec);

    if (end_sim == true) {
        return;
    }

    current_sim_time_usec += sim_dt_usec;
    current_sim_time_sec   = current_sim_time_usec / sec2usec;
}

template<typename DataBusType>
void SimSingleRun<DataBusType>::teardown() {
    actual_stop_time_sec     = current_sim_time_sec; 

    computer_stop_time       = std::chrono::high_resolution_clock::now();
    computer_elapsed_seconds = computer_stop_time - computer_start_time;
    sim_to_real_time_ratio   = actual_stop_time_sec / computer_elapsed_seconds.count();

    SimMetaData meta_data{
        config.start_time_sec,
        config.config_stop_time_sec,
        actual_stop_time_sec,
        stop_reason,
        stop_message,
        config.sim_rate_hz,
        config.total_mc_runs,
        config.current_mc_run,
        config.random_seed,
        computer_start_time,
        computer_stop_time,
        computer_elapsed_seconds,
        sim_to_real_time_ratio
    };

    sim_data_logger->log_sim_meta_data(meta_data);
    logger.close_file();
}
