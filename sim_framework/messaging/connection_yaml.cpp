#include "connection_yaml.hpp"

#include <sstream>
#include <stdexcept>

#include "../../utilities/yaml_utilities.hpp"

ConnectionYamlParser::ConnectionYamlParser(const std::string& owning_app_name, const std::string& path_to_config) {
    app_name    = owning_app_name;
    config_path = path_to_config;
}

void ConnectionYamlParser::parse_connections(std::array<IoConnectionSpec, SimConfig::max_connection_number>& specs,
                                             std::size_t& spec_count) {
    YAML::Node root;

    try {
        root = load_yaml_file(config_path);
    } catch (const std::runtime_error& error) {
        throw IoWiringError("[IO CONFIG] " + app_name + ": cannot read config file\n  " + error.what());
    }

    YAML::Node connections = root["connections"];

    // No block at all is legal: the app has no inputs, or only optional ones
    if (connections.IsDefined() == false || connections.IsNull()) {
        return;
    }

    if (connections.IsMap()) {
        record_error(line_label(line_of(connections)) +
                     "`connections:` is a map, not a list. The compact 'input: app.port' format is not "
                     "supported; write one entry per line:\n"
                     "         - { input: <my_input>, from_app: <publisher_app>, from_port: <publisher_port> }");
        throw_if_errors();
        return;
    }

    if (connections.IsSequence() == false) {
        record_error(line_label(line_of(connections)) +
                     "`connections:` must be a list of entries of the form\n"
                     "         - { input: <my_input>, from_app: <publisher_app>, from_port: <publisher_port> }");
        throw_if_errors();
        return;
    }

    for (std::size_t i = 0; i < connections.size(); i++) {
        parse_entry(connections[i], i + 1);
    }

    throw_if_errors();

    for (std::size_t i = 0; i < staged_count; i++) {
        if (spec_count == SimConfig::max_connection_number) {
            throw std::logic_error("[IO CONFIG] total connections exceed SimConfig::max_connection_number (" +
                                   std::to_string(SimConfig::max_connection_number) +
                                   "): increase the limit in sim_config.hpp");
        }

        specs[spec_count] = staged_specs[i];
        spec_count++;
    }
}

AppTlmSettings ConnectionYamlParser::parse_tlm_settings(double default_rate_hz) {
    YAML::Node root;

    try {
        root = load_yaml_file(config_path);
    } catch (const std::runtime_error& error) {
        throw IoWiringError("[IO CONFIG] " + app_name + ": cannot read config file\n  " + error.what());
    }

    AppTlmSettings settings;
    settings.app_name = app_name;
    settings.rate_hz  = default_rate_hz;

    YAML::Node rate_node = root["tlm_rate_hz"];

    if (rate_node.IsDefined() && rate_node.IsNull() == false) {
        bool converted = false;

        if (rate_node.IsScalar()) {
            try {
                settings.rate_hz = rate_node.as<double>();
                converted = true;
            } catch (const YAML::Exception&) {
                converted = false;
            }
        }

        if (converted == false) {
            record_error(line_label(line_of(rate_node)) + "`tlm_rate_hz` must be a single number, got '" +
                         (rate_node.IsScalar() ? rate_node.as<std::string>() : std::string("a non-scalar value")) + "'");
        } else if (settings.rate_hz <= 0.0) {
            record_error(line_label(line_of(rate_node)) + "`tlm_rate_hz` must be greater than zero, got " +
                         std::to_string(settings.rate_hz));
        }
    }

    YAML::Node level_node = root["tlm_level"];

    if (level_node.IsDefined() && level_node.IsNull() == false) {
        std::string level_text = level_node.IsScalar() ? level_node.as<std::string>() : "";

        if (level_text == "off") {
            settings.level = TlmLevel::Off;
        } else if (level_text == "required") {
            settings.level = TlmLevel::Required;
        } else if (level_text == "debug") {
            settings.level = TlmLevel::Debug;
        } else {
            std::string suggestion;
            const char* valid_levels[3] = {"off", "required", "debug"};

            for (std::size_t i = 0; i < 3; i++) {
                if (IoRegistry::edit_distance(level_text, valid_levels[i]) <= 2) {
                    suggestion = valid_levels[i];
                }
            }

            record_error(line_label(line_of(level_node)) + "`tlm_level` must be off, required, or debug; got '" +
                         level_text + "'" + (suggestion.empty() ? "" : "\n         did you mean '" + suggestion + "'?"));
        }
    }

    throw_if_errors();

    return settings;
}

void ConnectionYamlParser::parse_entry(const YAML::Node& entry, std::size_t entry_number) {
    std::size_t line = line_of(entry);

    if (entry.IsMap() == false) {
        record_error(line_label(line) + "entry " + std::to_string(entry_number) +
                     " is not a set of named fields; write it as\n"
                     "         - { input: <my_input>, from_app: <publisher_app>, from_port: <publisher_port> }");
        return;
    }

    check_entry_keys(entry, line);

    IoConnectionSpec spec;
    spec.destination_app = app_name;
    spec.origin          = config_path + ":" + std::to_string(line);

    bool input_ok     = extract_field(entry, line, "input", spec.input);
    bool from_app_ok  = extract_field(entry, line, "from_app", spec.from_app);
    bool from_port_ok = extract_field(entry, line, "from_port", spec.from_port);

    if (input_ok && from_app_ok && from_port_ok) {
        check_duplicate_input(spec.input, line);

        staged_specs[staged_count] = spec;
        staged_count++;
    }
}

// Rejects unknown and duplicate keys so a misspelled or stray field can never
// be silently ignored
void ConnectionYamlParser::check_entry_keys(const YAML::Node& entry, std::size_t line) {
    std::array<std::size_t, 3> key_counts{};

    for (YAML::const_iterator it = entry.begin(); it != entry.end(); ++it) {
        if (it->first.IsScalar() == false) {
            record_error(line_label(line) + "entry contains a key that is not a plain name");
            continue;
        }

        std::string key = it->first.as<std::string>();
        bool known = false;

        for (std::size_t i = 0; i < 3; i++) {
            if (key == required_keys[i]) {
                key_counts[i]++;
                known = true;
            }
        }

        if (known == false) {
            std::string suggestion = suggest_entry_key(key);
            record_error(line_label(line) + "unknown key '" + key + "' (valid keys: input, from_app, from_port)" +
                         (suggestion.empty() ? "" : "\n         did you mean '" + suggestion + "'?"));
        }
    }

    for (std::size_t i = 0; i < 3; i++) {
        if (key_counts[i] > 1) {
            record_error(line_label(line) + "key '" + std::string(required_keys[i]) +
                         "' appears " + std::to_string(key_counts[i]) + " times in one entry");
        }
    }
}

bool ConnectionYamlParser::extract_field(const YAML::Node& entry, std::size_t line, const char* key,
                                         std::string& value_out) {
    YAML::Node value = entry[key];

    if (value.IsDefined() == false) {
        record_error(line_label(line) + "entry is missing required key '" + key +
                     "' (found: " + joined_entry_keys(entry) + ")");
        return false;
    }

    if (value.IsNull()) {
        record_error(line_label(line) + "key '" + std::string(key) + "' has no value");
        return false;
    }

    if (value.IsScalar() == false) {
        record_error(line_label(line) + "the value of '" + std::string(key) +
                     "' must be a single name, not a list or nested fields");
        return false;
    }

    value_out = value.as<std::string>();

    std::string violation = IoRegistry::io_name_violation(value_out);

    if (violation.empty() == false) {
        record_error(line_label(line) + "value '" + value_out + "' of key '" + std::string(key) +
                     "' is invalid: " + violation);
        return false;
    }

    return true;
}

void ConnectionYamlParser::check_duplicate_input(const std::string& input_name, std::size_t line) {
    for (std::size_t i = 0; i < seen_input_count; i++) {
        if (seen_inputs[i] == input_name) {
            record_error(line_label(line) + "input '" + input_name +
                         "' is connected more than once (first connected at line " +
                         std::to_string(seen_input_lines[i]) + ")");
            return;
        }
    }

    if (seen_input_count < SimConfig::max_connection_number) {
        seen_inputs[seen_input_count]      = input_name;
        seen_input_lines[seen_input_count] = line;
        seen_input_count++;
    }
}

void ConnectionYamlParser::record_error(const std::string& message) {
    if (stored_error_count < parse_errors.size()) {
        parse_errors[stored_error_count] = message;
        stored_error_count++;
    }

    total_error_count++;
}

void ConnectionYamlParser::throw_if_errors() {
    if (total_error_count == 0) {
        return;
    }

    std::ostringstream report;
    report << "[IO CONFIG] " << app_name << " (" << config_path << "): " << total_error_count << " error(s):";

    if (total_error_count > stored_error_count) {
        report << "  (showing first " << stored_error_count << ")";
    }
    report << "\n";

    for (std::size_t i = 0; i < stored_error_count; i++) {
        report << "\n  " << (i + 1) << ". " << parse_errors[i] << "\n";
    }

    throw IoWiringError(report.str());
}

std::size_t ConnectionYamlParser::line_of(const YAML::Node& node) {
    // yaml-cpp marks are 0-based and -1 when unavailable
    if (node.Mark().line < 0) {
        return 0;
    }

    return static_cast<std::size_t>(node.Mark().line) + 1;
}

std::string ConnectionYamlParser::line_label(std::size_t line) {
    if (line == 0) {
        return "";
    }

    return "line " + std::to_string(line) + ": ";
}

std::string ConnectionYamlParser::joined_entry_keys(const YAML::Node& entry) {
    std::string joined;

    for (YAML::const_iterator it = entry.begin(); it != entry.end(); ++it) {
        if (it->first.IsScalar() == false) {
            continue;
        }

        if (!joined.empty()) {
            joined += ", ";
        }
        joined += it->first.as<std::string>();
    }

    return joined.empty() ? "no keys" : joined;
}

std::string ConnectionYamlParser::suggest_entry_key(const std::string& unknown_key) {
    std::size_t best_distance = 3;  // small keyset, keep suggestions tight
    std::string best_match;

    for (std::size_t i = 0; i < 3; i++) {
        std::size_t distance = IoRegistry::edit_distance(unknown_key, required_keys[i]);

        if (distance < best_distance) {
            best_distance = distance;
            best_match = required_keys[i];
        }
    }

    return best_match;
}
