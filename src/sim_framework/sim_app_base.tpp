#include "sim_app_base.hpp"
#include <iostream>
#include <string>

template<typename DataBusType>
SimAppBase<DataBusType>::SimAppBase(double execution_rate_hz, int schedule_priority, const std::string &path_to_config) {
  app_dt_sec         = 1.0 / execution_rate_hz;
  app_dt_usec        = static_cast<uint32_t>(sec2usec * app_dt_sec);
  next_run_time_usec = 0;
  priority           = schedule_priority;
  config_path        = path_to_config;
};

template<typename DataBusType>
void SimAppBase<DataBusType>::initialize(DataBusType& data_bus) { 
  this->configure_model(config_path, data_bus); 
};

template<typename DataBusType>
void SimAppBase<DataBusType>::check_step(const uint32_t &sim_time_usec) {
  time_to_step = sim_time_usec % app_dt_usec == 0;

  if (time_to_step == false) {
    return;
  }

  this->step();
};