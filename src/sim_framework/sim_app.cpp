#include "sim_app.hpp"
#include <iostream>
#include <string>

SimApp::SimApp(std::shared_ptr<SimAppBaseInterface> pointer_to_app,
               double execution_rate_hz, int schedule_priority,
               const std::string &path_to_config) {
  app = pointer_to_app;
  app_dt_sec = 1.0 / execution_rate_hz;
  app_dt_usec = static_cast<uint32_t>(sec2usec * app_dt_sec);
  next_run_time_usec = 0;
  priority = schedule_priority;
  config_path = path_to_config;
};

void SimApp::initialize(DataBus& data_bus) { 
  app->initialize(config_path, data_bus); 
};

void SimApp::step(const uint32_t &sim_time_usec) {
  time_to_step = sim_time_usec % app_dt_usec == 0;

  if (time_to_step == false) {
    return;
  }

  app->step();
};