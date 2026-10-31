#include "app_runner.hpp"

#include <sstream>
#include <stdexcept>

AppRunner::AppRunner(SimAppBase& app_to_run) {
    app         = &app_to_run;
    app_dt_usec = static_cast<uint64_t>(sec2usec * app_to_run.dt_sec());
}

void AppRunner::check_rate(double sim_rate_hz) const {
    uint64_t sim_dt_usec = static_cast<uint64_t>((1.0 / sim_rate_hz) * sec2usec);

    throw_if_app_runs_faster_than_sim(sim_dt_usec, sim_rate_hz);
    throw_if_period_is_not_whole_sim_steps(sim_dt_usec, sim_rate_hz);
}

void AppRunner::initialize(SimControl& sim_ctrl) {
    check_rate(sim_ctrl.public_sim_data().sim_rate_hz);

    app->configure_model(app->config_file(), sim_ctrl); // User defined code
}

void AppRunner::throw_if_app_runs_faster_than_sim(uint64_t sim_dt_usec, double sim_rate_hz) const {
    bool app_runs_faster_than_sim = app_dt_usec < sim_dt_usec;

    if (app_runs_faster_than_sim == true) {
        std::ostringstream message;
        message << app->name() << ": rate of " << 1.0 / app->dt_sec() << " Hz is faster than the sim rate of "
                << sim_rate_hz << " Hz. An app cannot step more often than the sim loop runs";
        throw std::logic_error(message.str());
    }
}

void AppRunner::throw_if_period_is_not_whole_sim_steps(uint64_t sim_dt_usec, double sim_rate_hz) const {
    bool period_is_not_whole_sim_steps = app_dt_usec % sim_dt_usec != 0;

    if (period_is_not_whole_sim_steps == true) {
        std::ostringstream message;
        message << app->name() << ": rate of " << 1.0 / app->dt_sec() << " Hz (period " << app_dt_usec
                << " usec) does not fit the sim rate of " << sim_rate_hz << " Hz (step " << sim_dt_usec
                << " usec). The app's period must be a whole number of sim steps or it never steps on schedule";
        throw std::logic_error(message.str());
    }
}

void AppRunner::attach_io(IoRegistry& registry, const AppIoInfo& plan) {
    io_registry = &registry;
    io_plan     = &plan;
}

void AppRunner::check_step(const uint64_t& sim_time_usec, SimControl& sim_ctrl) {
    bool time_to_step = sim_time_usec % app_dt_usec == 0;

    if (time_to_step == false) {
        return;
    }

    io_registry->copy_data(*io_plan);
    app->step(sim_ctrl); // User defined code
    io_registry->time_stamp_outputs(*io_plan, sim_time_usec);
}
