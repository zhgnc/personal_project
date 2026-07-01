#include "sim_single_run.hpp"

template<typename DataBusType>
SimSingleRun<DataBusType>::SimSingleRun(const SimSingleRunConfig<DataBusType>& config_data) {
    
    config = std::move(config_data);

    stop_time_usec = static_cast<uint64_t>(config.stop_time_sec * sec2usec);
    sim_dt_usec    = static_cast<uint64_t>((1.0 / config.sim_rate_hz) * sec2usec);

    current_sim_time_sec  = config.start_time_sec;
    current_sim_time_usec = static_cast<uint64_t>(config.start_time_sec * sec2usec);
    sim_step_count        = 0;

    // Copy unique apps and logging apps into the sim_single_run
    for (std::size_t i = 0; i < config.app_count; i++) {
        apps[i] = std::move(config.apps[i]);
    }

    for (std::size_t i = 0; i < config.logging_app_count; i++) {
        logging_apps[i] = std::move(config.logging_apps[i]);
    }
}

template<typename DataBusType>
void SimSingleRun<DataBusType>::run() {
    setup();

    while (current_sim_time_usec <= stop_time_usec && stop_type == StopType::NoStop) {
        step();
    }

    teardown();
}



template<typename DataBusType>
void SimSingleRun<DataBusType>::setup() {

    std::string file_name = std::format("{}_RUN_{:05}.hdf5", config.base_file_name, config.run_number);
    std::string full_path = config.output_directory + "/" + file_name;

    logger = std::make_unique<Logger>(full_path);
    LoggerFacade facade(*logger);

    for (std::size_t i = 0; i < config.logging_app_count; i++) {
        logging_apps[i]->configure_hdf5_logging(facade, config.data_bus, config.logging_rates);
    }

    sim_data_logger = std::make_unique<SimDataLogger>(*logger);
    sim_data_logger->configure_file_with_sim_data(current_sim_time_sec,
                                                  current_sim_time_usec,
                                                  config.sim_rate_hz,
                                                  sim_step_count);

    SimControl sim_ctrl = make_ctrl();

    for (std::size_t i = 0; i < config.app_count; i++) {
        apps[i]->initialize(sim_ctrl);
    }
}



template<typename DataBusType>
void SimSingleRun<DataBusType>::step() {

    accessible_sim_data = {current_sim_time_sec,
                           config.start_time_sec,
                           config.sim_rate_hz,
                           sim_step_count};

    SimControl sim_ctrl = make_ctrl();

    for (std::size_t i = 0; i < config.app_count; i++) {
        apps[i]->check_step(current_sim_time_usec, config.data_bus, sim_ctrl);

        if (stop_type == StopType::AfterApp) {
            return;
        }
    }

    logger->log_data(current_sim_time_usec);

    current_sim_time_usec += sim_dt_usec;
    current_sim_time_sec   = current_sim_time_usec / sec2usec;
    sim_step_count++;
}



template<typename DataBusType>
void SimSingleRun<DataBusType>::teardown() {

    SimControl sim_ctrl = make_ctrl();

    for (std::size_t i = 0; i < config.app_count; i++) {
        apps[i]->teardown(config.data_bus, sim_ctrl);
    }

    if (sim_data_logger) {
        sim_data_logger->log_sim_meta_data({
            config.start_time_sec,
            config.stop_time_sec,
            stop_type,
            stop_reason,
            stop_message,
            config.sim_rate_hz,
            config.run_number});
    }

    logger.reset();
    sim_data_logger.reset();
}


template<typename DataBusType>
SimControl SimSingleRun<DataBusType>::make_ctrl() {
    return SimControl(
        accessible_sim_data,
        [this](StopType t, StopReason r, const std::string& m) {
            stop_type = t;
            stop_reason = r;
            stop_message = m;
            end_sim(t, r, m);
        },
        [this]() { return get_seed(); },
        *logger,
        config.run_number
    );
}