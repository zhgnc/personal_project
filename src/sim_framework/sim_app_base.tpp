#include "sim_app_base.hpp"
#include <iostream>
#include <string>

template<typename DataBusType>
SimAppBase<DataBusType>::SimAppBase(std::string app_of_name, double execution_rate_hz, int schedule_priority, const std::string &path_to_config) {
  app_name     = app_of_name;
  app_dt_sec   = 1.0 / execution_rate_hz;
  app_dt_usec  = static_cast<uint64_t>(sec2usec * app_dt_sec);
  app_priority = schedule_priority;
  config_path  = path_to_config;
};

template<typename DataBusType>
void SimAppBase<DataBusType>::initialize(SimControl& sim_ctrl) { 
  this->configure_model(config_path, sim_ctrl); 
};

template<typename DataBusType>
void SimAppBase<DataBusType>::check_step(const uint64_t &sim_time_usec, DataBusType& bus, SimControl& sim_ctrl) {
  time_to_step = sim_time_usec % app_dt_usec == 0;

  if (time_to_step == false) {
    return;
  }

  this->step(bus, sim_ctrl);
};