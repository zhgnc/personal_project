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

    // Implemented by every app

    // Build the model from its config file. Called once per run, before declare_io()
    virtual void configure_model(const std::string& path_to_config, SimControl& sim_ctrl) = 0;

    // Bind port names to members of the model built in configure_model(). The
    // only place io.sub, io.sub_optional, io.tlm, and io.tlm_debug can be called
    virtual void declare_io(IoDeclarations& io) = 0;

    // Advance the model by one of this app's time steps. Subscribed inputs
    // already hold fresh values when this is called
    virtual void step(SimControl& sim_ctrl) = 0;

    // Final call before the run ends
    virtual void teardown(SimControl& sim_ctrl) = 0;

    const std::string& name() const { return app_name; }
    int priority() const { return app_priority; }
    double dt_sec() const { return app_dt_sec; }
    const std::string& config_file() const { return config_path; }

    // Sim Framework API (called by SimSingleRun)

    // Checks this app's rate fits the sim loop, then calls configure_model()
    void initialize(SimControl& sim_ctrl);

    // Hands the app its resolved copy plan once wiring is complete
    void attach_io(IoRegistry& registry, const AppIoInfo& plan);

    // Called every frame. Steps the app only when the sim time is a multiple of
    // its period, copying its inputs in first and stamping its outputs after
    void check_step(const uint64_t& sim_time_usec, SimControl& sim_ctrl);

protected:
    // Input queries for derived apps. Valid from the first step onward, not
    // inside configure_model() or declare_io()

    // False for a sub_optional() input that nothing connected, so the model is
    // running on its default value
    bool input_connected(const std::string& port_name) const;

    // Sim time the app feeding this input last stepped, so a model can tell how
    // old its data is. Max value until that app first steps, or when unconnected
    uint64_t input_last_update_usec(const std::string& port_name) const;

private:
    void throw_if_app_runs_faster_than_sim(uint64_t sim_dt_usec, double sim_rate_hz) const;
    void throw_if_period_is_not_whole_sim_steps(uint64_t sim_dt_usec, double sim_rate_hz) const;

    const PortRecord& find_own_input(const std::string& port_name) const;

    uint64_t app_dt_usec;
    std::string config_path;
    int app_priority;
    double app_dt_sec;
    std::string app_name;

    // Set by attach_io() once wiring resolves. Null on the prototype instances
    // SimManager stores, which are cloned but never stepped
    IoRegistry* io_registry  = nullptr;
    const AppIoInfo* io_plan = nullptr;

    static constexpr double sec2usec = 1e6;
};

#endif
