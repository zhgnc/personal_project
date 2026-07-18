#ifndef SIM_APP_BASE_HPP
#define SIM_APP_BASE_HPP

#include <cstdint>
#include <string>

#include "../sim_core/sim_control.hpp"
#include "../messaging/io_registry.hpp"

// Forward declare SimSingleRun so that it can be a friend class. This allows
// the `SimSingleRun` class to access the `initialize`, `check_step`, and
// `attach_io` functions without exposing the methods to the user when
// creating their own apps by inheriting from the `SimAppBase` class
class SimSingleRun;


class SimAppBase {
public:
    SimAppBase(std::string app_of_name,
               double execution_rate_hz,
               int schedule_priority,
               const std::string& path_to_config);

    virtual void configure_model(const std::string& path_to_config, SimControl& sim_ctrl) = 0;

    // Declare this app's subscribed inputs and published outputs (tlm_req /
    // debug_tlm). Called once per run on the cloned app instance, after
    // configure_model(), so every port binds to this run's own model members.
    // The default declares no IO
    virtual void declare_io(IoRegistry& io);

    virtual void step(SimControl& sim_ctrl) = 0;
    virtual void teardown(SimControl& sim_ctrl) = 0;

    virtual ~SimAppBase() = default;

    const std::string& name() const { return app_name; }
    int priority() const { return app_priority; }
    double dt_sec() const { return app_dt_sec; }
    const std::string& config_file() const { return config_path; }

protected:
    // IO queries for derived apps, scoped to this app's own declared ports.
    // Valid once wiring is resolved (from the first step onward), not inside
    // configure_model() or declare_io()
    bool input_connected(const std::string& port_name) const;

    // Sim time the connected source app last stepped; PortRecord::never_written
    // until the source's first step, or if the input is unconnected
    uint64_t input_last_update_usec(const std::string& port_name) const;

private:
    friend class SimSingleRun;

    void initialize(SimControl& sim_ctrl);
    void check_step(const uint64_t& sim_time_usec, SimControl& sim_ctrl);
    void attach_io(IoRegistry& registry, const AppIoPlan& plan);
    const PortRecord& find_own_port(const std::string& port_name) const;

    uint64_t app_dt_usec;
    bool time_to_step;
    std::string config_path;
    int app_priority;
    double app_dt_sec;
    std::string app_name;

    // Set per run by SimSingleRun once wiring resolves; always null on the
    // prototype instances that SimManager stores
    IoRegistry* io_registry  = nullptr;
    const AppIoPlan* io_plan = nullptr;

    static constexpr double sec2usec = 1e6;
};

#endif
