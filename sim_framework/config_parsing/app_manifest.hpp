#ifndef APP_MANIFEST_HPP
#define APP_MANIFEST_HPP

// Parses the app manifest, the one file that says which apps run. One line
// per app, every key required:
//
//   apps:
//     - { name: gyro_app, class: GyroSimApp, enabled: true, priority: 2, rate_hz: 5.0, config: gyro_config.yaml }
//
// `priority` sets the execution order (lower steps first) and must be unique
// among enabled apps. `config` is relative to the manifest's own folder, or
// "" for an app with no config file. Every problem in the file is reported at
// once, and no entry is kept unless the whole file is clean.

#include <array>
#include <cstddef>
#include <string>

#include <yaml-cpp/yaml.h>

#include "yaml_parser_base.hpp"
#include "../sim_config.hpp"

struct AppManifestEntry {
    std::string instance_name;
    std::string class_name;
    bool        enabled  = false;
    int         priority = 0;
    double      rate_hz  = 0.0;
    std::string config_path;
};

class AppManifestParser : public YamlParserBase {
public:
    explicit AppManifestParser(const std::string& path_to_manifest);

    // Fills `entries` in manifest order, or throws IoWiringError listing every
    // problem in the file
    void parse(std::array<AppManifestEntry, SimConfig::max_app_number>& entries, std::size_t& entry_count);

private:
    void parse_entry(const YAML::Node& entry, std::size_t entry_number);
    bool extract_class_name(const YAML::Node& entry, std::size_t line, std::string& class_name_out);
    bool extract_positive_rate(const YAML::Node& entry, std::size_t line, const char* key, double& rate_out);
    bool check_name_not_already_used(const AppManifestEntry& entry, std::size_t line);
    bool check_priority_not_already_used(const AppManifestEntry& entry, std::size_t line);
    std::string resolve_config_path(const std::string& config_path) const;

    // Entries wait here until the whole file is known to be clean
    std::array<AppManifestEntry, SimConfig::max_app_number> staged_entries;
    std::array<std::size_t, SimConfig::max_app_number> staged_lines;
    std::size_t staged_count = 0;

    static constexpr const char* entry_keys[6] = {"name", "class", "enabled", "priority", "rate_hz", "config"};
};

#endif
