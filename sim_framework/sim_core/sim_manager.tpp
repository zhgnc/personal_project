#include "sim_manager.hpp"

template<typename DataBusType>
SimManager<DataBusType>::SimManager(const std::string& path_to_sim_config, DataBusType& bus)
    : data_bus(bus)
{
    // YAML or config parsing happens here
    // (keep this minimal, just assign values)

    auto config = load_yaml_file(path_to_sim_config);

    start_time_sec       = get_yaml_value<double>(config, "start_time_sec");
    stop_time_sec        = get_yaml_value<double>(config, "stop_time_sec");
    sim_rate_hz          = get_yaml_value<double>(config, "sim_rate_hz");
    num_mc_runs          = get_yaml_value<size_t>(config, "num_mc_runs");
    init_seed            = get_yaml_value<uint64_t>(config, "init_seed");

    base_file_name       = get_yaml_value<std::string>(config, "base_file_name");
    output_directory     = get_yaml_value<std::string>(config, "output_directory");

    current_seed = init_seed;
}

template<typename DataBusType>
template<typename AppType>
void SimManager<DataBusType>::add_app(AppType&& app) {
    static_assert(std::derived_from<AppType, SimAppBase<DataBusType>>);

    app_prototypes[app_count++] = SimAppPrototype<DataBusType>(std::forward<AppType>(app));
}

template<typename DataBusType>
template<typename LoggingAppType>
void SimManager<DataBusType>::add_logging_app(LoggingAppType&& app) {
    static_assert(std::derived_from<LoggingAppType, LoggingAppBase<DataBusType>>);

    logging_prototypes[logging_app_count++] = LoggingAppPrototype<DataBusType>(std::forward<LoggingAppType>(app));
}

template<typename DataBusType>
void SimManager<DataBusType>::run() {
    for (current_run = 0; current_run < num_mc_runs; current_run++) {
        SimSingleRunConfig<DataBusType> cfg;

        build_single_run_config(cfg);

        SimSingleRun<DataBusType> run(std::move(cfg));

        run.run();

        current_seed = get_seed();
    }
}

template<typename DataBusType>
void SimManager<DataBusType>::build_single_run_config(SimSingleRunConfig<DataBusType>& cfg) {
    // ---------------- scalar config ----------------
    cfg.data_bus        = data_bus;

    cfg.start_time_sec  = start_time_sec;
    cfg.stop_time_sec   = stop_time_sec;
    cfg.sim_rate_hz     = sim_rate_hz;

    cfg.base_file_name  = base_file_name;
    cfg.output_directory = output_directory;

    cfg.run_number      = current_mc_run;

    // ---------------- clone apps ----------------
    cfg.app_count = app_count;

    for (std::size_t i = 0; i < app_count; i++) {
        cfg.apps[i] = app_prototypes[i].create();
    }

    // ---------------- clone logging apps ----------------
    cfg.logging_app_count = logging_app_count;

    for (std::size_t i = 0; i < logging_app_count; i++) {
        cfg.logging_apps[i] =
            logging_prototypes[i].create();
    }
}

template<typename DataBusType>
uint64_t SimManager<DataBusType>::get_seed() {
    // very simple example (you can upgrade later to RNG pool)
    return current_seed++;
}