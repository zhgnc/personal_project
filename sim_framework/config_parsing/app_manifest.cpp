#include "app_manifest.hpp"

#include <cctype>
#include <filesystem>
#include <stdexcept>

AppManifestParser::AppManifestParser(const std::string& path_to_manifest) {
    yaml_utilities = YamlParserUtilities("[APP MANIFEST]", "App manifest", path_to_manifest);
}

AppManifest AppManifestParser::parse() {
    YAML::Node root = yaml_utilities.load_root();

    bool file_is_not_a_map = root.IsMap() == false;

    if (file_is_not_a_map == true) {
        yaml_utilities.record_error("The manifest must be a map with a single `apps:` list");
        yaml_utilities.throw_if_errors();
    }

    yaml_utilities.check_map_keys(root, yaml_utilities.line_of(root), root_keys, 1);

    YAML::Node apps = root["apps"];

    AppManifest app_manifest;

    bool apps_is_a_list = check_apps_is_a_list(apps);

    if (apps_is_a_list == true) {
        app_manifest = read_apps(apps);
    }

    yaml_utilities.throw_if_errors();

    return app_manifest;
}

bool AppManifestParser::check_apps_is_a_list(const YAML::Node& apps) {
    bool apps_is_missing = apps.IsDefined() == false || apps.IsNull() == true;

    if (apps_is_missing == true) {
        yaml_utilities.record_error("Missing required `apps:` list. Use `apps: []` for a manifest with no apps");
        return false;
    }

    if (apps.IsSequence() == false) {
        yaml_utilities.record_error(yaml_utilities.line_label(yaml_utilities.line_of(apps)) +
                                    "`apps:` must be a list, one entry per app");
        return false;
    }

    return true;
}

AppManifest AppManifestParser::read_apps(const YAML::Node& apps) {
    AppManifest app_manifest;

    for (std::size_t i = 0; i < apps.size(); i++) {
        AppManifestEntry app;

        bool entry_is_sound = parse_entry(apps[i], i + 1, app);

        if (entry_is_sound == false) {
            continue;
        }

        bool name_is_new     = check_name_not_already_used(app_manifest, app);
        bool priority_is_new = check_priority_not_already_used(app_manifest, app);

        if (name_is_new == false || priority_is_new == false) {
            continue;
        }

        bool app_storage_is_full = app_manifest.count >= SimConfig::max_app_number;

        if (app_storage_is_full == true) {
            throw std::logic_error(SimConfig::capacity_message("[APP MANIFEST] Number of apps",
                                                               "max_app_number", SimConfig::max_app_number));
        }

        app_manifest.entries[app_manifest.count] = app;
        app_manifest.count++;
    }

    return app_manifest;
}

// Every field is checked even after one fails, so the report lists them all
bool AppManifestParser::parse_entry(const YAML::Node& entry, std::size_t entry_number, AppManifestEntry& app_out) {
    std::size_t line = yaml_utilities.line_of(entry);

    bool entry_is_not_a_map = entry.IsMap() == false;

    if (entry_is_not_a_map == true) {
        yaml_utilities.record_error(yaml_utilities.line_label(line) + "App entry " + std::to_string(entry_number) +
                                    " is not a set of named fields; each entry needs the keys "
                                    "name, class, enabled, priority, rate_hz, config");
        return false;
    }

    yaml_utilities.check_map_keys(entry, line, entry_keys, 6);

    app_out.line = line;

    bool name_ok     = yaml_utilities.extract_name(entry, line, "name", app_out.instance_name);
    bool class_ok    = extract_class_name(entry, line, app_out.class_name);
    bool enabled_ok  = yaml_utilities.extract_bool(entry, line, "enabled", app_out.enabled);
    bool priority_ok = yaml_utilities.extract_int(entry, line, "priority", app_out.priority);
    bool rate_ok     = extract_positive_rate(entry, line, "rate_hz", app_out.rate_hz);
    bool config_ok   = extract_config_path(entry, line, app_out.config_path);

    bool entry_is_sound = name_ok == true && class_ok == true && enabled_ok == true &&
                          priority_ok == true && rate_ok == true && config_ok == true;

    return entry_is_sound;
}

// Class names are C++ identifiers (e.g. GyroSimApp), not lowercase app names
bool AppManifestParser::extract_class_name(const YAML::Node& entry, std::size_t line, std::string& class_name_out) {
    if (yaml_utilities.extract_string(entry, line, "class", class_name_out) == false) {
        return false;
    }

    bool valid_identifier = class_name_out.empty() == false &&
                            std::isdigit(static_cast<unsigned char>(class_name_out[0])) == 0;

    for (char character : class_name_out) {
        bool identifier_character = std::isalnum(static_cast<unsigned char>(character)) != 0 || character == '_';
        valid_identifier          = valid_identifier && identifier_character;
    }

    if (valid_identifier == false) {
        yaml_utilities.record_error(yaml_utilities.line_label(line) + "Value '" + class_name_out +
                                    "' of key 'class' is not a valid C++ class name");
        return false;
    }

    return true;
}

bool AppManifestParser::extract_positive_rate(const YAML::Node& entry, std::size_t line, const char* key,
                                              double& rate_out) {
    if (yaml_utilities.extract_double(entry, line, key, rate_out) == false) {
        return false;
    }

    bool rate_is_not_positive = rate_out <= 0.0;

    if (rate_is_not_positive == true) {
        yaml_utilities.record_error(yaml_utilities.line_label(line) + "`" + std::string(key) +
                                    "` must be greater than zero, got " + std::to_string(rate_out));
        return false;
    }

    return true;
}

// Every app needs a config file with `input_mapping:` and `app_config:`
bool AppManifestParser::extract_config_path(const YAML::Node& entry, std::size_t line, std::string& config_path_out) {
    if (yaml_utilities.extract_string(entry, line, "config", config_path_out) == false) {
        return false;
    }

    bool config_path_is_empty = config_path_out.empty() == true;

    if (config_path_is_empty == true) {
        yaml_utilities.record_error(yaml_utilities.line_label(line) + "`config` is empty. Every app needs a config "
                                    "file with `input_mapping:` and `app_config:` sections");
        return false;
    }

    config_path_out = resolve_config_path(config_path_out);
    return true;
}

// Connections address ports as app_name.port_name, so app names must be unique
bool AppManifestParser::check_name_not_already_used(const AppManifest& app_manifest, const AppManifestEntry& app) {
    for (std::size_t i = 0; i < app_manifest.count; i++) {
        const AppManifestEntry& earlier_app = app_manifest.entries[i];

        bool name_already_used = earlier_app.instance_name == app.instance_name;

        if (name_already_used == true) {
            yaml_utilities.record_error(yaml_utilities.line_label(app.line) + "App name '" + app.instance_name +
                                        "' is used more than once (first used at line " +
                                        std::to_string(earlier_app.line) + ")");
            return false;
        }
    }

    return true;
}

// Disabled apps never step, so only enabled apps need unique priorities
bool AppManifestParser::check_priority_not_already_used(const AppManifest& app_manifest, const AppManifestEntry& app) {
    if (app.enabled == false) {
        return true;
    }

    for (std::size_t i = 0; i < app_manifest.count; i++) {
        const AppManifestEntry& earlier_app = app_manifest.entries[i];

        bool priority_already_used = earlier_app.enabled == true && earlier_app.priority == app.priority;

        if (priority_already_used == true) {
            yaml_utilities.record_error(yaml_utilities.line_label(app.line) + "App '" + app.instance_name +
                                        "' has priority " + std::to_string(app.priority) + ", already used by '" +
                                        earlier_app.instance_name + "' at line " + std::to_string(earlier_app.line) +
                                        ": enabled apps need unique priorities so the execution order is unambiguous");
            return false;
        }
    }

    return true;
}

// Relative to the manifest so the project folder can move. Absolute paths are kept
std::string AppManifestParser::resolve_config_path(const std::string& config_path) const {
    std::filesystem::path written_path(config_path);

    bool path_is_absolute = written_path.is_absolute();

    if (path_is_absolute == true) {
        return written_path.generic_string();
    }

    std::filesystem::path manifest_directory = std::filesystem::path(yaml_utilities.path()).parent_path();

    return (manifest_directory / written_path).generic_string();
}
