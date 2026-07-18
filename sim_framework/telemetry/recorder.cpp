#include "recorder.hpp"

#include <stdexcept>

#include "../../math/math.hpp"

void Recorder::configure(Logger& logger,
                         const IoRegistry& registry,
                         const std::array<AppTlmSettings, SimConfig::max_app_number>& tlm_settings,
                         std::size_t app_count) {
    // Tracks which apps already have their HDF5 group created
    std::array<bool, SimConfig::max_app_number> group_created{};

    for (const PortRecord& record : registry.ports()) {
        const AppTlmSettings* settings = settings_for(record.owner_app, tlm_settings, app_count);

        if (settings == nullptr || port_is_recorded(record, *settings) == false) {
            continue;
        }

        for (std::size_t i = 0; i < app_count; i++) {
            if (tlm_settings[i].app_name == record.owner_app && group_created[i] == false) {
                logger.add_group(record.owner_app);
                group_created[i] = true;
            }
        }

        register_port(logger, record, settings->rate_hz);
    }
}

const AppTlmSettings* Recorder::settings_for(const std::string& app_name,
                                             const std::array<AppTlmSettings, SimConfig::max_app_number>& tlm_settings,
                                             std::size_t app_count) {
    for (std::size_t i = 0; i < app_count; i++) {
        if (tlm_settings[i].app_name == app_name) {
            return &tlm_settings[i];
        }
    }

    return nullptr;
}

bool Recorder::port_is_recorded(const PortRecord& record, const AppTlmSettings& settings) {
    if (record.kind == PortKind::Subscribe || settings.level == TlmLevel::Off) {
        return false;
    }

    if (record.kind == PortKind::DebugTlm && settings.level != TlmLevel::Debug) {
        return false;
    }

    return true;
}

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
