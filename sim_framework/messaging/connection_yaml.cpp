// Implementation of the per-app config file parser. See connection_yaml.hpp
// for the file format and the rules enforced here.

#include "connection_yaml.hpp"

#include <stdexcept>

ConnectionYamlParser::ConnectionYamlParser(const std::string& owning_app_name, const std::string& path_to_config)
    : YamlParserBase("[IO CONFIG]", owning_app_name, path_to_config) {}

void ConnectionYamlParser::parse_input_mapping(std::array<IoConnectionSpec, SimConfig::max_connection_number>& specs,
                                               std::size_t& spec_count) {
    YAML::Node root = load_root();

    if (root.IsMap() == false) {
        record_error("the config file must be a map with the two sections `input_mapping:` and `app_config:`");
        throw_if_errors();
        return;
    }

    check_top_level_keys(root);

    YAML::Node input_mapping = root["input_mapping"];

    if (input_mapping.IsDefined() == false) {
        record_error("missing required `input_mapping:` section. Use `input_mapping: []` for an app "
                     "with no subscribed inputs");
    } else if (input_mapping.IsNull()) {
        record_error(line_label(line_of(input_mapping)) +
                     "`input_mapping:` has no value. Use `input_mapping: []` for an app with no "
                     "subscribed inputs");
    } else if (input_mapping.IsMap()) {
        record_error(line_label(line_of(input_mapping)) +
                     "`input_mapping:` is a map, not a list; write one entry per line:\n"
                     "         - { input: <my_input>, from_app: <publisher_app>, from_port: <publisher_port> }");
    } else if (input_mapping.IsSequence() == false) {
        record_error(line_label(line_of(input_mapping)) +
                     "`input_mapping:` must be a list of entries of the form\n"
                     "         - { input: <my_input>, from_app: <publisher_app>, from_port: <publisher_port> }");
    } else {
        for (std::size_t i = 0; i < input_mapping.size(); i++) {
            parse_entry(input_mapping[i], i + 1);
        }
    }

    YAML::Node app_config = root["app_config"];

    if (app_config.IsDefined() == false) {
        record_error("missing required `app_config:` section. Use `app_config: {}` for an app "
                     "with no parameters");
    } else if (app_config.IsNull()) {
        record_error(line_label(line_of(app_config)) +
                     "`app_config:` has no value. Use `app_config: {}` for an app with no parameters");
    } else if (app_config.IsMap() == false) {
        record_error(line_label(line_of(app_config)) +
                     "`app_config:` must be a map of parameter names to values");
    }

    throw_if_errors();

    // Only reached when the whole file was clean, so a partly-parsed file
    // never contributes connections
    for (std::size_t i = 0; i < staged_count; i++) {
        if (spec_count == SimConfig::max_connection_number) {
            throw std::logic_error(SimConfig::capacity_message("[IO CONFIG] total connections across all apps",
                                                               "max_connection_number",
                                                               SimConfig::max_connection_number));
        }

        specs[spec_count] = staged_specs[i];
        spec_count++;
    }
}

// The only legal top-level keys are input_mapping and app_config. Retired
// keys from earlier config layouts get pointed migration hints instead of a
// generic unknown-key message
void ConnectionYamlParser::check_top_level_keys(const YAML::Node& root) {
    for (YAML::const_iterator it = root.begin(); it != root.end(); ++it) {
        if (it->first.IsScalar() == false) {
            record_error("the config file contains a top-level key that is not a plain name");
            continue;
        }

        std::string key = it->first.as<std::string>();
        std::size_t line = line_of(it->first);

        if (key == "input_mapping" || key == "app_config") {
            continue;
        }

        if (key == "connections") {
            record_error(line_label(line) + "`connections` was renamed to `input_mapping`");
        } else if (key == "tlm_level" || key == "tlm_rate_hz") {
            record_error(line_label(line) + "`" + key + "` moved to this app's entry in the app manifest");
        } else {
            record_error(line_label(line) + "unknown top-level key '" + key +
                         "': the only valid sections are `input_mapping:` and `app_config:` "
                         "(model parameters belong under `app_config:`)");
        }
    }
}

// Validates one `input_mapping:` entry and stages it. The entry is only
// staged when all three fields are sound, so a broken entry cannot become a
// half-formed connection later
void ConnectionYamlParser::parse_entry(const YAML::Node& entry, std::size_t entry_number) {
    std::size_t line = line_of(entry);

    if (entry.IsMap() == false) {
        record_error(line_label(line) + "entry " + std::to_string(entry_number) +
                     " is not a set of named fields; write it as\n"
                     "         - { input: <my_input>, from_app: <publisher_app>, from_port: <publisher_port> }");
        return;
    }

    check_map_keys(entry, line, entry_keys, 3);

    IoConnectionSpec spec;
    spec.destination_app = owner_name;
    spec.origin          = file_path + ":" + std::to_string(line);

    bool input_ok     = extract_name(entry, line, "input", spec.input);
    bool from_app_ok  = extract_name(entry, line, "from_app", spec.from_app);
    bool from_port_ok = extract_name(entry, line, "from_port", spec.from_port);

    if (input_ok && from_app_ok && from_port_ok) {
        check_duplicate_input(spec.input, line);

        if (staged_count == staged_specs.size()) {
            throw std::logic_error(SimConfig::capacity_message("[IO CONFIG] " + owner_name + ": input_mapping entries",
                                                               "max_connection_number",
                                                               SimConfig::max_connection_number));
        }

        staged_specs[staged_count] = spec;
        staged_count++;
    }
}

// An input connected twice in one file is ambiguous. The registry also
// catches this, but reporting it here names both line numbers
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
