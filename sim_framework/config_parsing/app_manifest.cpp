#include "app_manifest.hpp"

#include <cctype>
#include <filesystem>
#include <stdexcept>

AppManifestParser::AppManifestParser(const std::string& path_to_manifest)
    : YamlParserBase("[APP MANIFEST]", "app manifest", path_to_manifest) {}

void AppManifestParser::parse(std::array<AppManifestEntry, SimConfig::max_app_number>& entries,
                              std::size_t& entry_count) {
    YAML::Node root = load_root();

    bool file_is_not_a_map = root.IsMap() == false;

    if (file_is_not_a_map == true) {
        parse_report.record_error("the manifest must be a map with a single `apps:` list");
        throw_if_errors();
    }

    static constexpr const char* root_keys[1] = {"apps"};
    check_map_keys(root, line_of(root), root_keys, 1);

    YAML::Node apps = root["apps"];

    bool apps_is_missing = apps.IsDefined() == false || apps.IsNull() == true;

    if (apps_is_missing == true) {
        parse_report.record_error("missing required `apps:` list. Use `apps: []` for a manifest with no apps");
    } else if (apps.IsSequence() == false) {
        parse_report.record_error(line_label(line_of(apps)) + "`apps:` must be a list, one entry per app");
    } else {
        for (std::size_t i = 0; i < apps.size(); i++) {
            parse_entry(apps[i], i + 1);
        }
    }

    throw_if_errors();

    for (std::size_t i = 0; i < staged_count; i++) {
        entries[i] = staged_entries[i];
    }

    entry_count = staged_count;
}

// Every field is checked even after one fails, so the report lists them all
void AppManifestParser::parse_entry(const YAML::Node& entry, std::size_t entry_number) {
    std::size_t line = line_of(entry);

    bool entry_is_not_a_map = entry.IsMap() == false;

    if (entry_is_not_a_map == true) {
        parse_report.record_error(line_label(line) + "app entry " + std::to_string(entry_number) +
                                  " is not a set of named fields; each entry needs the keys "
                                  "name, class, enabled, priority, rate_hz, config");
        return;
    }

    check_map_keys(entry, line, entry_keys, 6);

    AppManifestEntry staged;

    bool name_ok     = extract_name(entry, line, "name", staged.instance_name);
    bool class_ok    = extract_class_name(entry, line, staged.class_name);
    bool enabled_ok  = extract_bool(entry, line, "enabled", staged.enabled);
    bool priority_ok = extract_int(entry, line, "priority", staged.priority);
    bool rate_ok     = extract_positive_rate(entry, line, "rate_hz", staged.rate_hz);
    bool config_ok   = extract_string(entry, line, "config", staged.config_path);

    bool entry_is_sound = name_ok == true && class_ok == true && enabled_ok == true &&
                          priority_ok == true && rate_ok == true && config_ok == true;

    if (entry_is_sound == false) {
        return;
    }

    bool name_is_new     = check_name_not_already_used(staged, line);
    bool priority_is_new = check_priority_not_already_used(staged, line);

    if (name_is_new == false || priority_is_new == false) {
        return;
    }

    bool app_has_config_file = staged.config_path.empty() == false;

    if (app_has_config_file == true) {
        staged.config_path = resolve_config_path(staged.config_path);
    }

    bool staging_is_full = staged_count >= SimConfig::max_app_number;

    if (staging_is_full == true) {
        throw std::logic_error(SimConfig::capacity_message("[APP MANIFEST] app entries",
                                                           "max_app_number", SimConfig::max_app_number));
    }

    staged_entries[staged_count] = staged;
    staged_lines[staged_count]   = line;
    staged_count++;
}

// Class names are C++ identifiers (e.g. GyroSimApp), not lowercase app names
bool AppManifestParser::extract_class_name(const YAML::Node& entry, std::size_t line, std::string& class_name_out) {
    if (extract_string(entry, line, "class", class_name_out) == false) {
        return false;
    }

    bool valid_identifier = class_name_out.empty() == false &&
                            std::isdigit(static_cast<unsigned char>(class_name_out[0])) == 0;

    for (char character : class_name_out) {
        bool identifier_character = std::isalnum(static_cast<unsigned char>(character)) != 0 || character == '_';
        valid_identifier          = valid_identifier && identifier_character;
    }

    if (valid_identifier == false) {
        parse_report.record_error(line_label(line) + "value '" + class_name_out +
                                  "' of key 'class' is not a valid C++ class name");
        return false;
    }

    return true;
}

bool AppManifestParser::extract_positive_rate(const YAML::Node& entry, std::size_t line, const char* key,
                                              double& rate_out) {
    if (extract_double(entry, line, key, rate_out) == false) {
        return false;
    }

    bool rate_is_not_positive = rate_out <= 0.0;

    if (rate_is_not_positive == true) {
        parse_report.record_error(line_label(line) + "`" + std::string(key) + "` must be greater than zero, got " +
                                  std::to_string(rate_out));
        return false;
    }

    return true;
}

// Connections address ports as app_name.port_name, so app names must be unique
bool AppManifestParser::check_name_not_already_used(const AppManifestEntry& entry, std::size_t line) {
    for (std::size_t i = 0; i < staged_count; i++) {
        bool name_already_used = staged_entries[i].instance_name == entry.instance_name;

        if (name_already_used == true) {
            parse_report.record_error(line_label(line) + "app name '" + entry.instance_name +
                                      "' is used more than once (first used at line " +
                                      std::to_string(staged_lines[i]) + ")");
            return false;
        }
    }

    return true;
}

// Disabled apps never step, so only enabled apps need unique priorities
bool AppManifestParser::check_priority_not_already_used(const AppManifestEntry& entry, std::size_t line) {
    if (entry.enabled == false) {
        return true;
    }

    for (std::size_t i = 0; i < staged_count; i++) {
        bool priority_already_used = staged_entries[i].enabled == true && staged_entries[i].priority == entry.priority;

        if (priority_already_used == true) {
            parse_report.record_error(line_label(line) + "app '" + entry.instance_name + "' has priority " +
                                      std::to_string(entry.priority) + ", already used by '" +
                                      staged_entries[i].instance_name + "' at line " + std::to_string(staged_lines[i]) +
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

    std::filesystem::path manifest_directory = std::filesystem::path(file_path).parent_path();

    return (manifest_directory / written_path).generic_string();
}
