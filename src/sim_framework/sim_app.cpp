#include "sim_app.hpp"

SimApp::SimApp(std::shared_ptr<SimAppBaseInterface> pointer_to_app, double execution_rate_hz, int schedule_priority) {
    app_dt_sec         = 1.0 / execution_rate_hz;
    app_dt_usec        = static_cast<uint32_t>(1e6 * app_dt_sec);
    next_run_time_usec = 0;
    schedule_priority  = schedule_priority;
}

void SimApp::initialize() {
    app->initialize();
};

void SimApp::step(const uint32_t& sim_time_usec) {
    time_to_step = sim_time_usec % app_dt_usec == 0;

    if (time_to_step == false) {
        return;
    }

    app->step();
};