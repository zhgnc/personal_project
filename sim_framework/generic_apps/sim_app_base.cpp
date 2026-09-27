// Implementation of the lifecycle and IO plumbing shared by every sim app.
// See sim_app_base.hpp for the class contract.

#include "sim_app_base.hpp"

#include <string>

SimAppBase::SimAppBase(std::string app_of_name, double execution_rate_hz, int schedule_priority, const std::string &path_to_config) {
  app_name     = app_of_name;
  app_dt_sec   = 1.0 / execution_rate_hz;
  app_dt_usec  = static_cast<uint64_t>(sec2usec * app_dt_sec);
  app_priority = schedule_priority;
  config_path  = path_to_config;
};

void SimAppBase::initialize(SimControl& sim_ctrl) {
  this->configure_model(config_path, sim_ctrl);
};

// The heartbeat of data passing. An app steps only on frames that land on a
// multiple of its own period; on those frames its subscribed inputs are
// snapshotted first, so the model sees values as of the moment it starts
// rather than values other apps write later in the same frame.
void SimAppBase::check_step(const uint64_t &sim_time_usec, SimControl& sim_ctrl) {
  bool time_to_step = sim_time_usec % app_dt_usec == 0;

  if (time_to_step == false) {
    return;
  }

  if (io_plan != nullptr) {
    io_registry->copy_inputs(*io_plan);
  }

  this->step(sim_ctrl);

  // Time-stamp after the step so subscribers can tell how old this app's
  // outputs are
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
  return find_own_port(port_name).is_connected();
};

uint64_t SimAppBase::input_last_update_usec(const std::string& port_name) const {
  const PortRecord& own_port = find_own_port(port_name);

  if (own_port.is_connected() == false) {
    return PortRecord::never_written;
  }

  // The input stores the index of its source record, so reaching the
  // publisher is a direct lookup rather than a search
  return io_registry->ports()[own_port.source_record_index].last_write_usec;
};
