#ifndef APP_MANIFEST_HPP
#define APP_MANIFEST_HPP

#include <array>
#include <cstddef>
#include <string>

#include "sim_config.hpp"
#include "../messaging/yaml_parser_base.hpp"

// One app instance as listed in the manifest. `instance_name` must be unique
// across the manifest; `class_name` must name a class in the project's
// AppClassList; `priority` must be unique among enabled apps
struct AppManifestEntry {
    std::string instance_name;
    std::string class_name;
    bool        enabled     = false;
    int         priority    = 0;
    double      rate_hz     = 0.0;
    TlmLevel    tlm_level   = TlmLevel::Off;
    double      tlm_rate_hz = 0.0;
    std::string config_path;
};

// Parses the app manifest: the single yaml file that defines which apps run
// in the simulation. One line per app, every field named:
//
//   apps:
//     - { name: gyro_app, class: GyroSimApp, enabled: true, priority: 2,
//         rate_hz: 5.0, tlm_level: debug, tlm_rate_hz: 5.0, config: path/to/gyro_config.yaml }
//
// The multi-line block spelling of the same map is also accepted.
//
// Every key is required — there are no defaults. `priority` sets the
// execution order (lower steps first) and must be unique among enabled apps,
// so the frame order is never ambiguous. A sequence (not a name-keyed map) is
// used so duplicate instance names are always visible to validation instead
// of being collapsed by the yaml parser.
//
// Disabled apps (`enabled: false`) stay in the manifest for error reporting:
// a connection that names a disabled app gets a specific message instead of
// "app does not exist".
class AppManifestParser : public YamlParserBase {
public:
    explicit AppManifestParser(const std::string& path_to_manifest);

    // Fills `entries` in manifest order. Collects every problem in the file
    // and throws IoWiringError listing them all; on throw nothing is written
    void parse(std::array<AppManifestEntry, SimConfig::max_app_number>& entries, std::size_t& entry_count);

private:
    void parse_entry(const YAML::Node& entry, std::size_t entry_number);
    bool extract_class_name(const YAML::Node& entry, std::size_t line, std::string& class_name_out);
    bool extract_tlm_level(const YAML::Node& entry, std::size_t line, TlmLevel& level_out);
    bool extract_positive_rate(const YAML::Node& entry, std::size_t line, const char* key, double& rate_out);
    std::string resolve_config_path(const std::string& config_path) const;
    void check_duplicate_instance_name(const std::string& instance_name, std::size_t line);

    // Two enabled apps sharing a priority leaves their frame order to the
    // sort implementation, so it is rejected rather than silently resolved
    void check_duplicate_priority(const AppManifestEntry& entry, std::size_t line);

    // Entries staged here until the whole file is known to be clean
    std::array<AppManifestEntry, SimConfig::max_app_number> staged_entries;
    std::size_t staged_count = 0;

    // Line each instance name first appeared on, for duplicate reports
    std::array<std::string, SimConfig::max_app_number> seen_names;
    std::array<std::size_t, SimConfig::max_app_number> seen_name_lines;
    std::size_t seen_name_count = 0;

    // Priorities of enabled apps, with the name and line that claimed each
    std::array<int, SimConfig::max_app_number> seen_priorities;
    std::array<std::string, SimConfig::max_app_number> seen_priority_names;
    std::array<std::size_t, SimConfig::max_app_number> seen_priority_lines;
    std::size_t seen_priority_count = 0;

    static constexpr const char* entry_keys[8] = {"name", "class", "enabled", "priority",
                                                  "rate_hz", "tlm_level", "tlm_rate_hz", "config"};
};

#endif
