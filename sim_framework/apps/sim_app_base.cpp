#include "sim_app_base.hpp"

#include <cstddef>
#include <limits>
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
    configure_model(config_path, sim_ctrl);
}

void SimAppBase::attach_io(IoRegistry& registry, const AppIoInfo& plan) {
    io_registry = &registry;
    io_plan     = &plan;
}

// Inputs are copied in immediately before the step, so the model sees values as
// of the moment it starts rather than anything another app writes later in the
// same frame. Outputs are stamped after, so subscribers can tell how old they are
void SimAppBase::check_step(const uint64_t& sim_time_usec, SimControl& sim_ctrl) {
    bool time_to_step = sim_time_usec % app_dt_usec == 0;

    if (time_to_step == false) {
        return;
    }

    io_registry->copy_data(*io_plan);
    step(sim_ctrl);
    io_registry->time_stamp_outputs(*io_plan, sim_time_usec);
}

bool SimAppBase::input_connected(const std::string& port_name) const {
    return find_own_input(port_name).is_connected();
}

uint64_t SimAppBase::input_last_update_usec(const std::string& port_name) const {
    const PortRecord& own_input = find_own_input(port_name);

    if (own_input.is_connected() == false) {
        return std::numeric_limits<uint64_t>::max();
    }

    // The input stores the index of the output feeding it, so reaching the
    // publisher is a direct lookup rather than a search
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
