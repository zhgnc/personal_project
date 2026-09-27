// Implementation of the app manifest parser. See app_manifest.hpp for the
// file format and the rules enforced here.

#include "app_manifest.hpp"

#include <cctype>
#include <filesystem>
#include <stdexcept>

AppManifestParser::AppManifestParser(const std::string& path_to_manifest)
    : YamlParserBase("[APP MANIFEST]", "app manifest", path_to_manifest) {}

void AppManifestParser::parse(std::array<AppManifestEntry, SimConfig::max_app_number>& entries,
                              std::size_t& entry_count) {
    YAML::Node root = load_root();

    if (root.IsMap() == false) {
        record_error("the manifest must be a map with a single `apps:` list");
        throw_if_errors();
        return;
    }

    // The only legal top-level key is `apps`; anything else is flagged so a
    // stray or misspelled section can never be silently ignored
    static constexpr const char* root_keys[1] = {"apps"};
    check_map_keys(root, line_of(root), root_keys, 1);

    YAML::Node apps = root["apps"];

    if (apps.IsDefined() == false || apps.IsNull()) {
        record_error("missing required `apps:` list. Use `apps: []` for a manifest that lists no apps "
                     "(all apps added in code)");
        throw_if_errors();
        return;
    }

    if (apps.IsSequence() == false) {
        record_error(line_label(line_of(apps)) +
                     "`apps:` must be a list: one `- name: ... class: ...` entry per app, "
                     "in execution order");
        throw_if_errors();
        return;
    }

    for (std::size_t i = 0; i < apps.size(); i++) {
        parse_entry(apps[i], i + 1);
    }

    throw_if_errors();

    // Only reached when the whole manifest was clean, so a partly-parsed
    // manifest never contributes apps
    for (std::size_t i = 0; i < staged_count; i++) {
        if (entry_count == SimConfig::max_app_number) {
            throw std::logic_error(SimConfig::capacity_message("[APP MANIFEST] number of apps",
                                                               "max_app_number", SimConfig::max_app_number));
        }

        entries[entry_count] = staged_entries[i];
        entry_count++;
    }
}

// Validates one app entry and stages it. All eight fields are extracted even
// if an early one fails, so a single run reports every problem in the entry
// rather than one per rebuild
void AppManifestParser::parse_entry(const YAML::Node& entry, std::size_t entry_number) {
    std::size_t line = line_of(entry);

    if (entry.IsMap() == false) {
        record_error(line_label(line) + "app entry " + std::to_string(entry_number) +
                     " is not a set of named fields; each entry needs the keys "
                     "name, class, enabled, priority, rate_hz, tlm_level, tlm_rate_hz, config");
        return;
    }

    check_map_keys(entry, line, entry_keys, 8);

    AppManifestEntry staged;

    bool name_ok     = extract_name(entry, line, "name", staged.instance_name);
    bool class_ok    = extract_class_name(entry, line, staged.class_name);
    bool enabled_ok  = extract_bool(entry, line, "enabled", staged.enabled);
    bool priority_ok = extract_int(entry, line, "priority", staged.priority);
    bool rate_ok     = extract_positive_rate(entry, line, "rate_hz", staged.rate_hz);
    bool level_ok    = extract_tlm_level(entry, line, staged.tlm_level);
    bool tlm_rate_ok = extract_positive_rate(entry, line, "tlm_rate_hz", staged.tlm_rate_hz);
    bool config_ok   = extract_string(entry, line, "config", staged.config_path);

    if (name_ok && class_ok && enabled_ok && priority_ok && rate_ok && level_ok && tlm_rate_ok && config_ok) {
        check_duplicate_instance_name(staged.instance_name, line);
        check_duplicate_priority(staged, line);

        staged.config_path = resolve_config_path(staged.config_path);

        if (staged_count == staged_entries.size()) {
            throw std::logic_error(SimConfig::capacity_message("[APP MANIFEST] app entries",
                                                               "max_app_number", SimConfig::max_app_number));
        }

        staged_entries[staged_count] = staged;
        staged_count++;
    }
}

// Config paths are written relative to the manifest that lists them, so an
// entry names just its own file and the whole project directory can move
// without editing every path. Absolute paths are used as written.
std::string AppManifestParser::resolve_config_path(const std::string& config_path) const {
    std::filesystem::path written_path(config_path);

    if (written_path.is_absolute()) {
        return written_path.generic_string();
    }

    std::filesystem::path manifest_directory = std::filesystem::path(file_path).parent_path();

    return (manifest_directory / written_path).generic_string();
}

// Class names are C++ identifiers (e.g. GyroSimApp), so they follow the
// identifier rule rather than the lowercase port-name rule
bool AppManifestParser::extract_class_name(const YAML::Node& entry, std::size_t line,
                                           std::string& class_name_out) {
    if (extract_string(entry, line, "class", class_name_out) == false) {
        return false;
    }

    bool valid = class_name_out.empty() == false &&
                 (std::isdigit(static_cast<unsigned char>(class_name_out[0])) == 0);

    for (char character : class_name_out) {
        bool identifier_char = (std::isalnum(static_cast<unsigned char>(character)) != 0) || character == '_';

        if (identifier_char == false) {
            valid = false;
        }
    }

    if (valid == false) {
        record_error(line_label(line) + "value '" + class_name_out +
                     "' of key 'class' is not a valid C++ class name");
        return false;
    }

    return true;
}

bool AppManifestParser::extract_tlm_level(const YAML::Node& entry, std::size_t line, TlmLevel& level_out) {
    std::string level_text;

    if (extract_string(entry, line, "tlm_level", level_text) == false) {
        return false;
    }

    if (tlm_level_from_string(level_text, level_out) == false) {
        static constexpr const char* valid_levels[3] = {"off", "required", "debug"};
        std::string suggestion = closest_key(level_text, valid_levels, 3);

        record_error(line_label(line) + "`tlm_level` must be off, required, or debug; got '" + level_text + "'" +
                     (suggestion.empty() ? "" : "\n         did you mean '" + suggestion + "'?"));
        return false;
    }

    return true;
}

bool AppManifestParser::extract_positive_rate(const YAML::Node& entry, std::size_t line, const char* key,
                                              double& rate_out) {
    if (extract_double(entry, line, key, rate_out) == false) {
        return false;
    }

    if (rate_out <= 0.0) {
        record_error(line_label(line) + "`" + std::string(key) + "` must be greater than zero, got " +
                     std::to_string(rate_out));
        return false;
    }

    return true;
}

// Instance names address ports as "app_name.port_name", so a duplicate would
// make every connection to that name ambiguous
void AppManifestParser::check_duplicate_instance_name(const std::string& instance_name, std::size_t line) {
    for (std::size_t i = 0; i < seen_name_count; i++) {
        if (seen_names[i] == instance_name) {
            record_error(line_label(line) + "app name '" + instance_name +
                         "' is used more than once (first used at line " +
                         std::to_string(seen_name_lines[i]) + "): instance names must be unique");
            return;
        }
    }

    if (seen_name_count < SimConfig::max_app_number) {
        seen_names[seen_name_count]      = instance_name;
        seen_name_lines[seen_name_count] = line;
        seen_name_count++;
    }
}

void AppManifestParser::check_duplicate_priority(const AppManifestEntry& entry, std::size_t line) {
    // Disabled apps never step, so their priorities cannot collide
    if (entry.enabled == false) {
        return;
    }

    for (std::size_t i = 0; i < seen_priority_count; i++) {
        if (seen_priorities[i] == entry.priority) {
            record_error(line_label(line) + "app '" + entry.instance_name + "' has priority " +
                         std::to_string(entry.priority) + ", already used by '" + seen_priority_names[i] +
                         "' at line " + std::to_string(seen_priority_lines[i]) +
                         ": enabled apps need unique priorities so the execution order is unambiguous");
            return;
        }
    }

    if (seen_priority_count < SimConfig::max_app_number) {
        seen_priorities[seen_priority_count]     = entry.priority;
        seen_priority_names[seen_priority_count] = entry.instance_name;
        seen_priority_lines[seen_priority_count] = line;
        seen_priority_count++;
    }
}
