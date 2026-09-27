#ifndef SIM_APP_BASE_HPP
#define SIM_APP_BASE_HPP

// SimAppBase is the class every simulation app inherits from — the framework's
// entire user-facing extension point. An app wraps one model, tells the
// framework what data it needs and produces, and advances the model each time
// its turn comes around.

#include <cstdint>
#include <string>

#include "../sim_core/sim_control.hpp"
#include "../messaging/io_registry.hpp"

// Forward declare SimSingleRun so that it can be a friend class. This allows
// the `SimSingleRun` class to access the `initialize`, `check_step`, and
// `attach_io` functions without exposing the methods to the user when
// creating their own apps by inheriting from the `SimAppBase` class
class SimSingleRun;


// Base class for every sim app. Derived apps override the four virtuals
// below; the framework owns the rest of the lifecycle.
//
// Per run, in order: configure_model() builds the model from config and
// dispersions, declare_io() binds ports to the model's members, the framework
// resolves the wiring, then check_step() drives the model every frame the
// app's rate calls for, and teardown() runs once at the end.
//
// Apps are cloned for every Monte Carlo run, so an app must keep all of its
// state in members — anything shared between runs would break run isolation.
class SimAppBase {
public:
    SimAppBase(std::string app_of_name,
               double execution_rate_hz,
               int schedule_priority,
               const std::string& path_to_config);

    // Build the model from its config file. Path to this app's config file
    // from the manifest, or an empty string for apps added in code. The
    // framework has already checked that the file parses and has an
    // `app_config:` section, so reading it here is safe.
    // Called once per run, before declare_io()
    virtual void configure_model(const std::string& path_to_config, SimControl& sim_ctrl) = 0;

    // Declare this app's inputs and outputs by binding port names to members
    // of the model built in configure_model(). This is the only place
    // io.sub/sub_optional/tlm_req/tlm_debug may be called
    virtual void declare_io(IoRegistry& io) = 0;

    // Advance the model by one of this app's time steps. Subscribed inputs
    // are already filled with fresh values when this is called
    virtual void step(SimControl& sim_ctrl) = 0;

    // Final call before the run ends, for anything the model needs to close
    // out or report
    virtual void teardown(SimControl& sim_ctrl) = 0;

    virtual ~SimAppBase() = default;

    const std::string& name() const { return app_name; }
    int priority() const { return app_priority; }
    double dt_sec() const { return app_dt_sec; }
    const std::string& config_file() const { return config_path; }

protected:
    // ---------------- IO queries for derived apps ----------------
    // Both are scoped to this app's own ports and are valid once wiring is
    // resolved (from the first step onward), not inside configure_model() or
    // declare_io()

    // False for a sub_optional() input that no config file connected, which
    // means the model is running on the default value
    bool input_connected(const std::string& port_name) const;

    // Sim time the app feeding this input last stepped, so a model can tell
    // how old its data is. PortRecord::never_written until the source's first
    // step, or if the input is unconnected
    uint64_t input_last_update_usec(const std::string& port_name) const;

private:
    friend class SimSingleRun;

    // Called once per run before declare_io()
    void initialize(SimControl& sim_ctrl);

    // Called every frame: steps the app only when the sim time is a multiple
    // of its period, and wraps the step in the IO copy/stamp sequence
    void check_step(const uint64_t& sim_time_usec, SimControl& sim_ctrl);

    // Hands the app its resolved copy plan once wiring is complete
    void attach_io(IoRegistry& registry, const AppIoPlan& plan);

    // Looks up one of this app's own ports for the query methods above
    const PortRecord& find_own_port(const std::string& port_name) const;

    uint64_t app_dt_usec;
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
