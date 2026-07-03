#include "sim_single_run.hpp"

template<typename DataBusType>
SimSingleRun<DataBusType>::SimSingleRun(SimSingleRunConfig<DataBusType>&& config_data) {
    data_bus          = config_data.data_bus;
    app_count         = config_data.app_count;
    logging_app_count = config_data.logging_app_count;
    
    for (std::size_t i = 0; i < app_count; i++) {
        apps[i] = std::move(config_data.apps[i]);
    }

    for (std::size_t i = 0; i < logging_app_count; i++) {
        logging_apps[i] = std::move(config_data.logging_apps[i]);
    }

    logging_rates         = config_data.logging_rates;
    print_hdf5_file_tree  = config_data.print_hdf5_file_tree;
    print_file_attributes = config_data.print_file_attributes;

    output_directory = config_data.output_directory;
    base_file_name   = config_data.base_file_name;

    start_time_sec = config_data.start_time_sec;
    stop_time_sec  = config_data.stop_time_sec;
    sim_rate_hz    = config_data.sim_rate_hz;

    run_number    = config_data.run_number;
    total_mc_runs = config_data.total_mc_runs;


    stop_time_usec = static_cast<uint64_t>(stop_time_sec * sec2usec);
    sim_dt_usec    = static_cast<uint64_t>((1.0 / sim_rate_hz) * sec2usec);

    current_sim_time_sec  = start_time_sec;
    current_sim_time_usec = static_cast<uint64_t>(start_time_sec * sec2usec);
    sim_step_count        = 1;   

    // TODO: Need to initialize stop tpes, reason, and message and add end_sim() and print_stop_diagnostics()
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
    std::cout << "[SimSingleRun] Starting Simulation Run #" << run_number << std::endl;

    std::string file_name = std::format("{}_RUN_{:05}.hdf5", base_file_name, run_number);
    std::string full_path = output_directory + "/" + file_name;

    logger = std::make_unique<Logger>(full_path);
    LoggerFacade logging_interface(*logger);

    for (std::size_t i = 0; i < logging_app_count; i++) {
        logging_apps[i]->configure_hdf5_logging(logging_interface, data_bus, logging_rates);
    }

    sim_data_logger = std::make_unique<SimDataLogger>(*logger);
    sim_data_logger->configure_file_with_sim_data(current_sim_time_sec,
                                                  current_sim_time_usec,
                                                  sim_rate_hz,
                                                  sim_step_count);
    // TODO: Refactor sim_ctrl
    SimControl sim_ctrl = make_ctrl();

    for (std::size_t i = 0; i < app_count; i++) {
        apps[i]->initialize(sim_ctrl); 
    }

    // TODO: Add in wall clock stats like start, stop. and elapsed time
    // TODO: Need to add in log_run_meta_data 
    // computer_start_time = std::chrono::high_resolution_clock::now();
}



template<typename DataBusType>
void SimSingleRun<DataBusType>::step() {

    accessible_sim_data = {current_sim_time_sec,
                           start_time_sec,
                           sim_rate_hz,
                           sim_step_count};

    SimControl sim_ctrl = make_ctrl();

    for (std::size_t i = 0; i < app_count; i++) {
        apps[i]->check_step(current_sim_time_usec, data_bus, sim_ctrl);

        if (stop_type == StopType::AfterApp) {
            return;
        }
    }

    logger->log_data(current_sim_time_usec);

    current_sim_time_usec += sim_dt_usec;
    current_sim_time_sec   = current_sim_time_usec / sec2usec;
    sim_step_count         = sim_step_count + 1;
}



template<typename DataBusType>
void SimSingleRun<DataBusType>::teardown() {

    SimControl sim_ctrl = make_ctrl();

    for (std::size_t i = 0; i < app_count; i++) {
        apps[i]->teardown(data_bus, sim_ctrl);
    }

    // TODO: Calculate wall time stats and print results
    // std::cout << "[SimSingleRun] Run #" << run_number << " ended after " << computer_elapsed_seconds.count() << "seconds (x" << sim_to_real_time_ratio << "faster than real time)\n";

    // TODO: Replace if statement with log_run_meta_data
    if (sim_data_logger) {
        sim_data_logger->log_sim_meta_data({
            start_time_sec,
            stop_time_sec,
            stop_type,
            stop_reason,
            stop_message,
            sim_rate_hz,
            run_number});
    }

    // If this is the last run I want to print the file tree for reference if enabled
    // which must be done before `.reset()` is called below
    if (run_number >= total_mc_runs && print_hdf5_file_tree == true) {
        sim_teardown(); // TODO: Move this to manager incase the last run does not finish last
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
        run_number
    );
}

// TODO: Make get_seed() apart of the single run class and verify it still works with sim_ctrl
template<typename DataBusType>
uint64_t SimSingleRun<DataBusType>::get_seed() {
  return current_seed++; // Returns value of `current_seed` and then increments by 1
}

template<typename DataBusType>
void SimSingleRun<DataBusType>::sim_teardown() {
  std::cout << "\n[Simulation] ALL RUNS FINISHED!!!\n\n";
    
  std::cout << "[Simulation] HDF5 output files will have the following data\n";
  logger->print_file_tree(print_file_attributes);
}