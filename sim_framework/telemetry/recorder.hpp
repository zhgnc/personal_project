#ifndef RECORDER_HPP
#define RECORDER_HPP

#include <array>
#include <cstddef>
#include <string>
#include <typeindex>

#include "../messaging/io_registry.hpp"
#include "../data_logging/logger.hpp"

// Registers HDF5 datasets for every recordable port in the registry: one
// group per app, one dataset per port, named exactly as declared in
// declare_io(). Which ports are recorded is controlled by each app's
// tlm_level (Off: none, Required: tlm_req only, Debug: tlm_req + debug_tlm),
// sampled at the app's tlm_rate_hz.
//
// The recorder is the only bridge between the type-erased port records and
// the typed Logger API, so it carries the list of recordable port types. A
// port whose type is not in that list is a startup error naming the port —
// extend register_port() to support a new type.
class Recorder {
public:
    static void configure(Logger& logger,
                          const IoRegistry& registry,
                          const std::array<AppTlmSettings, SimConfig::max_app_number>& tlm_settings,
                          std::size_t app_count);

private:
    static const AppTlmSettings* settings_for(const std::string& app_name,
                                              const std::array<AppTlmSettings, SimConfig::max_app_number>& tlm_settings,
                                              std::size_t app_count);

    static bool port_is_recorded(const PortRecord& record, const AppTlmSettings& settings);
    static void register_port(Logger& logger, const PortRecord& record, double rate_hz);

    // Registers the dataset when the record's payload type is T
    template<typename T>
    static bool try_register(Logger& logger, const PortRecord& record, double rate_hz) {
        if (record.type != std::type_index(typeid(T))) {
            return false;
        }

        logger.add_dataset(record.port_name, record.owner_app, *static_cast<const T*>(record.data_ptr), rate_hz);
        return true;
    }
};

#endif
