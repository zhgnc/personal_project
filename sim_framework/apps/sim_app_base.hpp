#ifndef SIM_APP_BASE_HPP
#define SIM_APP_BASE_HPP

// SimAppBase is the class every simulation app inherits from. An app wraps one
// model, tells the framework what data it needs and produces, and advances the
// model each time its turn comes around.

#include <cstdint>
#include <string>

#include "../sim_main_loop/sim_control.hpp"
#include "../messaging/io_declarations.hpp"

// Derived apps override the four virtuals below and the framework owns the rest.
//
// Per run, in order: configure_model() builds the model from config and
// dispersions, declare_io() binds ports to the model's members, the framework
// resolves the wiring, then check_step() drives the model every frame the
// app's rate calls for, and teardown() runs once at the end.
//
// Apps are cloned for every Monte Carlo run, so an app must keep all of its
// state in members. Anything shared between runs would break run isolation.
class SimAppBase {
public:
    SimAppBase(std::string app_of_name,
               double execution_rate_hz,
               int schedule_priority,
               const std::string& path_to_config);

    virtual ~SimAppBase() = default;

    // Virtual functions user defines
    virtual void configure_model(const std::string& path_to_config, SimControl& sim_ctrl) = 0;
    virtual void declare_io(IoDeclarations& io) = 0;
    virtual void step(SimControl& sim_ctrl) = 0;
    virtual void teardown(SimControl& sim_ctrl) = 0;

    // Getters for class variables 
    const std::string& name() const { return app_name; }
    int priority() const { return app_priority; }
    double dt_sec() const { return app_dt_sec; }
    const std::string& config_file() const { return config_path; }

    // API functions to sim framework
    void initialize(SimControl& sim_ctrl);
    void attach_io(IoRegistry& registry, const AppIoInfo& plan);
    void check_step(const uint64_t& sim_time_usec, SimControl& sim_ctrl);

protected:
    bool input_connected(const std::string& port_name) const;
    uint64_t time_last_update_usec(const std::string& port_name) const;

private:
    void throw_if_app_runs_faster_than_sim(uint64_t sim_dt_usec, double sim_rate_hz) const;
    void throw_if_period_is_not_whole_sim_steps(uint64_t sim_dt_usec, double sim_rate_hz) const;

    const PortRecord& find_own_input(const std::string& port_name) const;

    uint64_t app_dt_usec;
    std::string config_path;
    int app_priority;
    double app_dt_sec;
    std::string app_name;

    IoRegistry* io_registry  = nullptr;
    const AppIoInfo* io_plan = nullptr;

    static constexpr double sec2usec = 1e6;
};

#endif
