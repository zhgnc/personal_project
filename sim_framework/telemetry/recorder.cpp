// Implementation of the port-to-HDF5 bridge. See recorder.hpp for what the
// recorder is responsible for.

#include "recorder.hpp"

#include <stdexcept>

#include "../../math/math.hpp"

// Walks every declared port once and registers a dataset for each one the
// owning app's telemetry level admits. Groups are created lazily so an app
// whose ports are all filtered out leaves no empty group in the file
void Recorder::configure(Logger& logger,
                         const IoRegistry& registry,
                         const std::array<AppTlmSettings, SimConfig::max_app_number>& tlm_settings,
                         std::size_t app_count) {
    // Tracks which apps already have their HDF5 group created
    std::array<bool, SimConfig::max_app_number> group_created{};

    for (const PortRecord& record : registry.ports()) {
        std::size_t settings_index = settings_index_for(record.owner_app, tlm_settings, app_count);

        // Every declared port belongs to an app the manager configured, so a
        // missing entry means the settings array and the registry disagree
        if (settings_index == app_count) {
            throw std::logic_error("[Recorder] port '" + record.owner_app + "." + record.port_name +
                                   "' belongs to an app with no telemetry settings");
        }

        const AppTlmSettings& settings = tlm_settings[settings_index];

        if (port_is_recorded(record, settings) == false) {
            continue;
        }

        if (group_created[settings_index] == false) {
            logger.add_group(record.owner_app);
            group_created[settings_index] = true;
        }

        register_port(logger, record, settings.rate_hz);
    }
}

// Index of an app's telemetry settings, or `app_count` when it has none.
// Returning the index (not a pointer) lets the caller reuse it for the
// per-app group bookkeeping
std::size_t Recorder::settings_index_for(const std::string& app_name,
                                         const std::array<AppTlmSettings, SimConfig::max_app_number>& tlm_settings,
                                         std::size_t app_count) {
    for (std::size_t i = 0; i < app_count; i++) {
        if (tlm_settings[i].app_name == app_name) {
            return i;
        }
    }

    return app_count;
}

// The recording policy in one place: inputs are never recorded (their source
// output already is), and debug ports need the app to be at debug level
bool Recorder::port_is_recorded(const PortRecord& record, const AppTlmSettings& settings) {
    if (record.kind == PortKind::Subscribe || settings.level == TlmLevel::Off) {
        return false;
    }

    if (record.kind == PortKind::TlmDebug && settings.level != TlmLevel::Debug) {
        return false;
    }

    return true;
}

// Recovers the port's static type from its type_index by trying each
// supported type in turn: try_register returns false immediately unless the
// type matches, so exactly one call in the chain does the work. Supporting a
// new port type means adding one line here
void Recorder::register_port(Logger& logger, const PortRecord& record, double rate_hz) {
    bool registered =
        try_register<bool>(logger, record, rate_hz) ||
        try_register<int>(logger, record, rate_hz) ||
        try_register<double>(logger, record, rate_hz) ||
        try_register<uint64_t>(logger, record, rate_hz) ||
        try_register<vector<double, 3>>(logger, record, rate_hz) ||
        try_register<vector<double, 12>>(logger, record, rate_hz) ||
        try_register<quat<double>>(logger, record, rate_hz) ||
        try_register<rot_vec<double>>(logger, record, rate_hz);

    if (registered == false) {
        throw std::logic_error("[Recorder] port '" + record.owner_app + "." + record.port_name + "' has type " +
                               record.type_name + " which the recorder cannot store: add the type to "
                               "Recorder::register_port() in sim_framework/telemetry/recorder.cpp");
    }
}
