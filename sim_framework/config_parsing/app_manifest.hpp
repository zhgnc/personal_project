#ifndef APP_MANIFEST_HPP
#define APP_MANIFEST_HPP

// Parses the app manifest, the one file that says which apps run. One line
// per app, every key required:
//
//   apps:
//     - { name: gyro_app, class: GyroSimApp, enabled: true, priority: 2, rate_hz: 5.0, config: gyro_config.yaml }
//
// `priority` sets the execution order (lower steps first) and must be unique
// among enabled apps. `config` is relative to the manifest's own folder, and
// every app needs one. Every problem in the file is reported at once.

#include <array>
#include <cstddef>
#include <string>

#include <yaml-cpp/yaml.h>

#include "yaml_parser_utilities.hpp"
#include "../sim_config.hpp"

struct AppManifestEntry {
    std::string instance_name;
    std::string class_name;
    bool        enabled  = false;
    int         priority = 0;
    double      rate_hz  = 0.0;
    std::string config_path;
    std::size_t line = 0; // Manifest line, for error messages
};

// Every app in the manifest, in manifest order
struct AppManifest {
    std::array<AppManifestEntry, SimConfig::max_app_number> entries;
    std::size_t count = 0;
};

class AppManifestParser {
public:
    explicit AppManifestParser(const std::string& path_to_manifest);

    // Returns every app in the manifest, or throws IoWiringError listing every
    // problem in the file
    AppManifest parse();

private:
    bool check_apps_is_a_list(const YAML::Node& apps);
    AppManifest read_apps(const YAML::Node& apps);
    bool parse_entry(const YAML::Node& entry, std::size_t entry_number, AppManifestEntry& app_out);
    bool extract_class_name(const YAML::Node& entry, std::size_t line, std::string& class_name_out);
    bool extract_positive_rate(const YAML::Node& entry, std::size_t line, const char* key, double& rate_out);
    bool extract_config_path(const YAML::Node& entry, std::size_t line, std::string& config_path_out);
    bool check_name_not_already_used(const AppManifest& app_manifest, const AppManifestEntry& app);
    bool check_priority_not_already_used(const AppManifest& app_manifest, const AppManifestEntry& app);
    std::string resolve_config_path(const std::string& config_path) const;

    YamlParserUtilities yaml_utilities;

    static constexpr const char* root_keys[1]  = {"apps"};
    static constexpr const char* entry_keys[6] = {"name", "class", "enabled", "priority", "rate_hz", "config"};
};

#endif
