#include "sim_app_base.hpp"

#include <string>

SimAppBase::SimAppBase(std::string app_of_name, double execution_rate_hz, int schedule_priority, const std::string &path_to_config) {
  app_name     = app_of_name;
  app_dt_sec   = 1.0 / execution_rate_hz;
  app_dt_usec  = static_cast<uint64_t>(sec2usec * app_dt_sec);
  app_priority = schedule_priority;
  config_path  = path_to_config;
};

void SimAppBase::declare_io(IoRegistry& io) {
  (void)io; // Default: app declares no IO; override to subscribe and publish ports
};

void SimAppBase::initialize(SimControl& sim_ctrl) {
  this->configure_model(config_path, sim_ctrl);
};

void SimAppBase::check_step(const uint64_t &sim_time_usec, SimControl& sim_ctrl) {
  time_to_step = sim_time_usec % app_dt_usec == 0;

  if (time_to_step == false) {
    return;
  }

  // Snapshot this app's subscribed inputs, run the model, then time-stamp its
  // outputs so subscribers and telemetry can see data age
  if (io_plan != nullptr) {
    io_registry->copy_inputs(*io_plan);
  }

  this->step(sim_ctrl);

  if (io_plan != nullptr) {
    io_registry->stamp_outputs(*io_plan, sim_time_usec);
  }
};

void SimAppBase::attach_io(IoRegistry& registry, const AppIoPlan& plan) {
  io_registry = &registry;
  io_plan     = &plan;
};

const PortRecord& SimAppBase::find_own_port(const std::string& port_name) const {
  if (io_registry == nullptr) {
    throw std::logic_error(app_name + ": IO queries are not available until wiring is resolved "
                           "(valid from the first step onward, not in configure_model() or declare_io())");
  }

  for (const PortRecord& record : io_registry->ports()) {
    if (record.owner_app == app_name && record.port_name == port_name) {
      return record;
    }
  }

  throw std::logic_error(app_name + ": IO query on unknown port '" + port_name + "'");
};

bool SimAppBase::input_connected(const std::string& port_name) const {
  return find_own_port(port_name).is_connected;
};

uint64_t SimAppBase::input_last_update_usec(const std::string& port_name) const {
  const PortRecord& own_port = find_own_port(port_name);

  if (own_port.is_connected == false) {
    return PortRecord::never_written;
  }

  for (const PortRecord& record : io_registry->ports()) {
    if (record.owner_app + "." + record.port_name == own_port.connected_source) {
      return record.last_write_usec;
    }
  }

  return PortRecord::never_written;
};
