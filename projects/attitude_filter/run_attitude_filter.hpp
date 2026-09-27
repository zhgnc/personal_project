#ifndef RUN_ATTITUDE_FILTER_HPP
#define RUN_ATTITUDE_FILTER_HPP

#include "sim_framework/sim_includes.hpp"

#include "sim_files/fake_dynamics_sim_app.hpp"
#include "sim_files/gyro_sim_app.hpp"
#include "sim_files/star_tracker_sim_app.hpp"
#include "sim_files/attitude_filter_sim_app.hpp"

// Every sim app class this project makes available to its manifest. The
// manifest's `class:` values select from this list by class name; which
// instances actually run, their rates, telemetry, and wiring all live in
// the config files (see app_manifest.yaml)
inline const AppClassList attitude_filter_apps = {
    app_class<FakeDynamicsSimApp>(),
    app_class<GyroSimApp>(),
    app_class<StarTrackerSimApp>(),
    app_class<AttitudeFilterSimApp>(),
};

int run_attitude_filter() {
    SimManager sim("projects/attitude_filter/config_files/sim_config.yaml", attitude_filter_apps);

    sim.run();

    return 0;
};

#endif
