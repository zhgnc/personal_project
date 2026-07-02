#include "sim_manager.hpp"

template<typename DataBusType>
SimManager<DataBusType>::SimManager(const std::string& path_to_sim_config, DataBusType& bus) : data_bus(bus) {
    // TODO: Consider making this function take a struct rather than yaml file
    YAML::Node config = load_yaml_file(path_to_sim_config);

    start_time_sec       = get_yaml_value<double>(config, "sim_start_time_sec");
    stop_time_sec        = get_yaml_value<double>(config, "sim_stop_time_sec");
    sim_rate_hz          = get_yaml_value<double>(config, "simulation_rate_hz");
    num_mc_runs          = get_yaml_value<size_t>(config, "number_of_monte_carlo_runs");
    init_seed            = get_yaml_value<uint64_t>(config, "initial_random_seed");
    // TODO: missing print_hdf5_file_format & print_hdf5_attributes_in_file_format: false

    base_file_name       = get_yaml_value<std::string>(config, "base_file_name");
    output_directory     = get_yaml_value<std::string>(config, "logging_file_save_directory");

    app_count         = 0;
    logging_app_count = 0;

    // TODO: Need to add back in logging rates or see if they are read in somewhere else
}

template<typename DataBusType>
template<typename AppType>
void SimManager<DataBusType>::add_app(AppType&& new_app) {
    static_assert(std::is_base_of_v<SimAppBase<DataBusType>, std::decay_t<AppType>>,
                 "AppType must derive from SimAppBase<DataBusType>");
    
    if (app_count >= SimConfig::max_app_number) {
        throw std::runtime_error("[Simulation.tpp] Number of apps exceeded value of `max_app_number` in sim_config.hpp which is "
                                + std::to_string(SimConfig::max_app_number));
    }

    // TODO: Need comments explaining function like I did before
    app_prototypes[app_count] = SimAppPrototype<DataBusType>(std::forward<AppType>(new_app));
    app_count = app_count + 1;
}

template<typename DataBusType>
template<typename LoggingAppType>
void SimManager<DataBusType>::add_logging_app(LoggingAppType&& new_logging_app) {
    static_assert(std::is_base_of_v<LoggingAppBase<DataBusType>, std::decay_t<LoggingAppType>>,
                 "LoggingAppType must derive from LoggingAppBase<DataBusType>");

    if (logging_app_count >= SimConfig::max_logging_app_number) {
        throw std::runtime_error("[Simulation.tpp] Number of logging apps exceeded value of `max_logging_app_number` in sim_config.hpp which is"
                                + std::to_string(SimConfig::max_logging_app_number));
    }

    logging_prototypes[logging_app_count] = LoggingAppPrototype<DataBusType>(std::forward<LoggingAppType>(new_logging_app));
    logging_app_count = logging_app_count + 1;
}

template<typename DataBusType>
void SimManager<DataBusType>::run() {
    // TODO: Need to add back in sort apps by priority
    std::cout << "[SimManager] Configuring Simulation\n";

    // TODO: Make for loop run in parallel

    // For loop configured so that run_num starts at 1
    for (std::size_t run_num = 1; run_num < num_mc_runs + 1; run_num++) {
        SimSingleRunConfig<DataBusType> single_run_config = build_single_run_config(run_num);

        // SimSingleRun<DataBusType> single_run(std::move(single_run_config));
        SimSingleRun<DataBusType> single_run(single_run_config); // TODO: Verify if std::move is required

        single_run.run();
    }
}

template<typename DataBusType>
SimSingleRunConfig<DataBusType> SimManager<DataBusType>::build_single_run_config(const std::size_t& run_number) {    
    SimSingleRunConfig<DataBusType> run_config;
    
    run_config.data_bus         = data_bus;
    run_config.start_time_sec   = start_time_sec;
    run_config.stop_time_sec    = stop_time_sec;
    run_config.sim_rate_hz      = sim_rate_hz;
    run_config.base_file_name   = base_file_name;
    run_config.output_directory = output_directory;
    run_config.run_number       = run_number;
    // TODO: Need to add logging rates here for AppLoggingRates logging_rates

    run_config.app_count = app_count;

    for (std::size_t i = 0; i < app_count; i++) {
        run_config.apps[i] = app_prototypes[i].create();
    }

    run_config.logging_app_count = logging_app_count;

    for (std::size_t i = 0; i < logging_app_count; i++) {
        run_config.logging_apps[i] = logging_prototypes[i].create();
    }

    return run_config;
}

// TODO: Make get_seed() apart of the single run class and verify it still works with sim_ctrl
template<typename DataBusType>
uint64_t SimManager<DataBusType>::get_seed() {
    // very simple example (you can upgrade later to RNG pool)
    return current_seed++;
}