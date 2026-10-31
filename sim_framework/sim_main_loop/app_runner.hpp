#ifndef APP_RUNNER_HPP
#define APP_RUNNER_HPP

// Runs one app for SimSingleRun: checks its rate, wires its IO, and steps it on
// schedule. Function does not live in SimAppBase so users cannot call any of it.

#include <cstdint>

#include "../apps/sim_app_base.hpp"

class AppRunner {
public:
    AppRunner() = default; 
    explicit AppRunner(SimAppBase& app_to_run);

    void initialize(SimControl& sim_ctrl);
    void attach_io(IoRegistry& registry, const AppIoInfo& plan);
    void check_step(const uint64_t& sim_time_usec, SimControl& sim_ctrl);

private:
    void throw_if_app_runs_faster_than_sim(uint64_t sim_dt_usec, double sim_rate_hz) const;
    void throw_if_period_is_not_whole_sim_steps(uint64_t sim_dt_usec, double sim_rate_hz) const;

    SimAppBase* app          = nullptr;
    uint64_t app_dt_usec     = 0;
    IoRegistry* io_registry  = nullptr;
    const AppIoInfo* io_plan = nullptr;

    static constexpr double sec2usec = 1e6;
};

#endif
