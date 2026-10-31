#include "sim_app_base.hpp"

#include <cstddef>
#include <limits>
#include <sstream>
#include <stdexcept>

SimAppBase::SimAppBase(std::string app_of_name,
                       double execution_rate_hz,
                       int schedule_priority,
                       const std::string& path_to_config) {
    app_name     = app_of_name;
    app_dt_sec   = 1.0 / execution_rate_hz;
    app_dt_usec  = static_cast<uint64_t>(sec2usec * app_dt_sec);
    app_priority = schedule_priority;
    config_path  = path_to_config;
}

void SimAppBase::initialize(SimControl& sim_ctrl) {
    const SimControl::AccessibleSimData& sim_data = sim_ctrl.public_sim_data();

    uint64_t sim_dt_usec = static_cast<uint64_t>((1.0 / sim_data.sim_rate_hz) * sec2usec);

    throw_if_app_runs_faster_than_sim(sim_dt_usec, sim_data.sim_rate_hz);
    throw_if_period_is_not_whole_sim_steps(sim_dt_usec, sim_data.sim_rate_hz);

    configure_model(config_path, sim_ctrl); // User defined code
}

void SimAppBase::throw_if_app_runs_faster_than_sim(uint64_t sim_dt_usec, double sim_rate_hz) const {
    bool app_runs_faster_than_sim = app_dt_usec < sim_dt_usec;

    if (app_runs_faster_than_sim == true) {
        std::ostringstream message;
        message << app_name << ": rate of " << 1.0 / app_dt_sec << " Hz is faster than the sim rate of "
                << sim_rate_hz << " Hz. An app cannot step more often than the sim loop runs";
        throw std::logic_error(message.str());
    }
}

void SimAppBase::throw_if_period_is_not_whole_sim_steps(uint64_t sim_dt_usec, double sim_rate_hz) const {
    bool period_is_not_whole_sim_steps = app_dt_usec % sim_dt_usec != 0;

    if (period_is_not_whole_sim_steps == true) {
        std::ostringstream message;
        message << app_name << ": rate of " << 1.0 / app_dt_sec << " Hz (period " << app_dt_usec
                << " usec) does not fit the sim rate of " << sim_rate_hz << " Hz (step " << sim_dt_usec
                << " usec). The app's period must be a whole number of sim steps or it never steps on schedule";
        throw std::logic_error(message.str());
    }
}

void SimAppBase::attach_io(IoRegistry& registry, const AppIoInfo& plan) {
    io_registry = &registry;
    io_plan     = &plan;
}

void SimAppBase::check_step(const uint64_t& sim_time_usec, SimControl& sim_ctrl) {
    bool time_to_step = sim_time_usec % app_dt_usec == 0;

    if (time_to_step == false) {
        return;
    }

    io_registry->copy_data(*io_plan);
    step(sim_ctrl); // User defined code
    io_registry->time_stamp_outputs(*io_plan, sim_time_usec);
}

bool SimAppBase::input_connected(const std::string& port_name) const {
    return find_own_input(port_name).is_connected();
}

uint64_t SimAppBase::time_last_update_usec(const std::string& port_name) const {
    const PortRecord& own_input = find_own_input(port_name);

    if (own_input.is_connected() == false) {
        return std::numeric_limits<uint64_t>::max();
    }

    return io_registry->port_at(own_input.source_numerical_index).last_write_usec;
}

const PortRecord& SimAppBase::find_own_input(const std::string& port_name) const {
    bool wiring_not_resolved_yet = io_registry == nullptr;

    if (wiring_not_resolved_yet == true) {
        throw std::logic_error(app_name + ": input queries are not available until wiring is resolved "
                               "(valid from the first step onward, not in configure_model() or declare_io())");
    }

    for (std::size_t i = 0; i < io_registry->port_count(); i++) {
        const PortRecord& record = io_registry->port_at(i);
        bool is_requested_input  = record.owner_app == app_name && record.port_name == port_name &&
                                   record.is_input() == true;

        if (is_requested_input == true) {
            return record;
        }
    }

    throw std::logic_error(app_name + ": '" + port_name + "' is not one of this app's inputs");
}
