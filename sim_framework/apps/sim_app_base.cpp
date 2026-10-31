#include "sim_app_base.hpp"

SimAppBase::SimAppBase(std::string app_of_name,
                       double execution_rate_hz,
                       int schedule_priority,
                       const std::string& path_to_config) {
    app_name     = app_of_name;
    app_dt_sec   = 1.0 / execution_rate_hz;
    app_priority = schedule_priority;
    config_path  = path_to_config;
}
