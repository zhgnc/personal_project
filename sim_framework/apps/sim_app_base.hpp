#ifndef SIM_APP_BASE_HPP
#define SIM_APP_BASE_HPP

// SimAppBase is the class every simulation app inherits from. An app wraps one
// model, tells the framework what data it needs and produces, and advances the
// model each time its turn comes around.

#include <string>

#include "../sim_main_loop/sim_control.hpp"
#include "../messaging/io_declarations.hpp"

// Derived apps override the four virtuals below. Everything the framework does
// to run an app lives in AppRunner, which apps never see.
//
// Per run, in order: configure_model() builds the model from config and
// dispersions, declare_io() binds ports to the model's members, the framework
// resolves the wiring, then step() runs every frame the app's rate calls for,
// and teardown() runs once at the end.
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

private:
    std::string config_path;
    int app_priority;
    double app_dt_sec;
    std::string app_name;
};

#endif
