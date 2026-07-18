#include "sim_single_run.hpp"

#include <format>

SimSingleRun::SimSingleRun(SimSingleRunConfig&& config_data) {
    print_hdf5_file_tree  = config_data.print_hdf5_file_tree;
    print_file_attributes = config_data.print_file_attributes;
    output_directory      = config_data.output_directory;
    base_file_name        = config_data.base_file_name;
    start_time_sec        = config_data.start_time_sec;
    stop_time_sec         = config_data.stop_time_sec;
    sim_rate_hz           = config_data.sim_rate_hz;
    run_number            = config_data.run_number;
    total_mc_runs         = config_data.total_mc_runs;
    initial_seed          = config_data.initial_random_seed;
    app_count             = config_data.app_count;

    stop_time_usec        = static_cast<uint64_t>(stop_time_sec * sec2usec);
    sim_dt_sec            = 1.0 / sim_rate_hz;
    sim_dt_usec           = static_cast<uint64_t>((1.0 / sim_rate_hz) * sec2usec);
    current_sim_time_sec  = start_time_sec;
    current_sim_time_usec = static_cast<uint64_t>(start_time_sec * sec2usec);
    sim_step_count        = 0;
    stop_type             = StopType::NoStop;
    stop_reason           = StopReason::ReachedConfiguredStopTime;
    stop_message          = "None Provided";
    actual_stop_time_sec  = stop_time_sec;

    for (std::size_t i = 0; i < app_count; i++) {
        apps[i] = std::move(config_data.apps[i]);
        tlm_settings[i] = std::move(config_data.tlm_settings[i]);
    }

    io_connection_count = config_data.io_connection_count;

    for (std::size_t i = 0; i < io_connection_count; i++) {
        io_connections[i] = std::move(config_data.io_connections[i]);
    }
}

SimRunStats SimSingleRun::run() {
    setup();

    while (current_sim_time_usec <= stop_time_usec && stop_type == StopType::NoStop) {
        step();
    }

    teardown();

    return create_run_stats();
}

SimRunStats SimSingleRun::create_run_stats() {
    SimRunStats run_stats;

    run_stats.run_number             = run_number;
    run_stats.sim_time_sec           = actual_stop_time_sec - start_time_sec;
    run_stats.wall_clock_sec         = computer_elapsed_seconds.count();
    run_stats.sim_to_real_time_ratio = sim_to_real_time_ratio;
    run_stats.stop_type              = stop_type;
    run_stats.stop_reason            = stop_reason;
    run_stats.stop_message           = stop_message;

    return run_stats;
}



void SimSingleRun::setup() {
    {
        std::lock_guard<std::mutex> lock(console_mutex);
        std::cout << "[SimSingleRun] Starting run " << run_number << " of " << total_mc_runs << "\n";

    }

    std::string file_name = std::format("{}_RUN_{:05}.hdf5", base_file_name, run_number);
    std::string full_path = output_directory + "/" + file_name;

    logger = std::make_unique<Logger>(full_path);

    sim_data_logger = std::make_unique<SimDataLogger>(*logger);

    sim_data_logger->configure_file_with_sim_data(current_sim_time_sec,
                                                  current_sim_time_usec,
                                                  sim_rate_hz,
                                                  sim_step_count);

    // TODO: Refactor sim_ctrl
    update_accessible_sim_data();
    SimControl sim_ctrl = make_ctrl();

    for (std::size_t i = 0; i < app_count; i++) {
        apps[i]->initialize(sim_ctrl);
    }

    setup_io();

    // The recorder registers HDF5 datasets straight from the resolved port
    // records, so it must run after setup_io()
    Recorder::configure(*logger, *io_registry, tlm_settings, app_count);

    computer_start_time = std::chrono::high_resolution_clock::now();
}

// Collects every app's IO declarations, replays the connection specs, and
// resolves the topology for this run's cloned app instances. Runs after
// configure_model() so declared ports bind to fully configured models.
void SimSingleRun::setup_io() {
    io_registry = std::make_unique<IoRegistry>();

    for (std::size_t i = 0; i < app_count; i++) {
        io_registry->begin_declarations(apps[i]->name());
        apps[i]->declare_io(*io_registry);
        io_registry->end_declarations();
    }

    for (std::size_t i = 0; i < io_connection_count; i++) {
        io_registry->connect(io_connections[i].destination_app,
                             io_connections[i].input,
                             io_connections[i].from_app,
                             io_connections[i].from_port,
                             io_connections[i].origin);
    }

    io_registry->resolve_and_validate();

    // Attach each app to its plan and snapshot all inputs once so every app
    // sees its publishers' initialization values before the first step
    for (std::size_t i = 0; i < app_count; i++) {
        const AppIoPlan& app_plan = io_registry->plan_for(apps[i]->name());

        apps[i]->attach_io(*io_registry, app_plan);
        io_registry->copy_inputs(app_plan);
    }
}



void SimSingleRun::step() {

    update_accessible_sim_data();
    SimControl sim_ctrl = make_ctrl();

    for (std::size_t i = 0; i < app_count; i++) {
        apps[i]->check_step(current_sim_time_usec, sim_ctrl);

        if (stop_type == StopType::AfterApp) {
            return;
        }
    }
    logger->log_data(current_sim_time_usec);

    current_sim_time_usec += sim_dt_usec;
    current_sim_time_sec   = current_sim_time_usec / sec2usec;
    sim_step_count         = sim_step_count + 1;
}



void SimSingleRun::teardown() {
    update_accessible_sim_data();
    SimControl sim_ctrl = make_ctrl();

    for (std::size_t i = 0; i < app_count; i++) {
        apps[i]->teardown(sim_ctrl);
    }

    computer_stop_time       = std::chrono::high_resolution_clock::now();
    computer_elapsed_seconds = computer_stop_time - computer_start_time;
    sim_to_real_time_ratio   = actual_stop_time_sec / computer_elapsed_seconds.count();

    {
        std::lock_guard<std::mutex> lock(console_mutex);
        std::cout << "[Simulation] Run #" << run_number << " ended after " << computer_elapsed_seconds.count() << "seconds (x" << sim_to_real_time_ratio << "faster than real time)\n";
    }

    log_run_meta_data();

    // If this is the last run I want to print the file tree for reference if enabled
    // which must be done before `.reset()` is called below
    if (run_number >= total_mc_runs && print_hdf5_file_tree == true) {
        sim_teardown(); // TODO: Move this to manager incase the last run does not finish last
    }

    logger.reset();
    sim_data_logger.reset();
}


SimControl SimSingleRun::make_ctrl() {
    return SimControl(accessible_sim_data,
                      [this](StopType t, StopReason r, const std::string& m) { end_sim(t, r, m); },
                      [this]() { return get_seed(); },
                      *logger,
                      run_number);
}

void SimSingleRun::update_accessible_sim_data() {
    accessible_sim_data = SimControl::AccessibleSimData{current_sim_time_sec, sim_dt_sec, sim_rate_hz, sim_step_count};
}

uint64_t SimSingleRun::get_seed() {
    uint64_t seed_output = run_number + current_seed;
    current_seed = current_seed + 1;

    return seed_output; // Returns value of `current_seed` and then increments by 1
}

void SimSingleRun::end_sim(const StopType& type, const StopReason& reason, const std::string& message) {
    if (stop_type != StopType::NoStop) {
      return;
    }

    stop_type            = type;
    stop_reason          = reason;
    stop_message         = message;
    actual_stop_time_sec = current_sim_time_sec;

    print_stop_diagnostics(type, reason, message);
}


void SimSingleRun::print_stop_diagnostics(const StopType& type, const StopReason& reason, const std::string& message) {
  std::string type_string   = stop_type_to_string(type);
  int type_int              = static_cast<int>(type);
  std::string reason_string = stop_reason_to_string(reason);
  int reason_int            = static_cast<int>(reason);

  std::lock_guard<std::mutex> lock(console_mutex);

  std::cout << "               | - Sim stop commanded\n";
  std::cout << "                   |- Type (enum): "   << type_string   << " || " << type_int   << std::endl;
  std::cout << "                   |- Reason (enum): " << reason_string << " || " << reason_int << std::endl;
  std::cout << "                   |- Message: " << message << std::endl;
}

void SimSingleRun::log_run_meta_data() {
    meta_data.start_time_sec           = start_time_sec;
    meta_data.config_stop_time_sec     = stop_time_sec;
    meta_data.actual_stop_time_sec     = actual_stop_time_sec;
    meta_data.stop_type                = stop_type;
    meta_data.stop_reason              = stop_reason;
    meta_data.stop_message             = stop_message;
    meta_data.sim_rate_hz              = sim_rate_hz;
    meta_data.num_total_mc_runs        = total_mc_runs;
    meta_data.current_mc_run           = run_number;
    meta_data.initial_random_seed      = initial_seed;
    meta_data.computer_start_time      = computer_start_time;
    meta_data.computer_stop_time       = computer_stop_time;
    meta_data.computer_elapsed_seconds = computer_elapsed_seconds;
    meta_data.sim_to_real_time_ratio   = sim_to_real_time_ratio;
    meta_data.app_count                = app_count;

    sim_data_logger->log_sim_meta_data(meta_data);
}

void SimSingleRun::sim_teardown() {
  std::lock_guard<std::mutex> lock(console_mutex);

  std::cout << "\n[Simulation] ALL RUNS FINISHED!!!\n\n";

  std::cout << "[Simulation] HDF5 output files will have the following data\n";
  logger->print_file_tree(print_file_attributes);
}
