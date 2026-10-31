#include "connection_yaml.hpp"

#include <stdexcept>

ConnectionYamlParser::ConnectionYamlParser(const std::string& owning_app_name, const std::string& path_to_config)
    : YamlParserBase("[IO CONFIG]", owning_app_name, path_to_config) {}

void ConnectionYamlParser::parse_input_mapping(std::array<IoConfigSpec, SimConfig::max_connection_number>& specs,
                                               std::size_t& spec_count) {
    YAML::Node root = load_root();

    bool file_is_not_a_map = root.IsMap() == false;

    if (file_is_not_a_map == true) {
        parse_report.record_error("the config file must be a map with the two sections `input_mapping:` and `app_config:`");
        throw_if_errors();
    }

    check_top_level_keys(root);
    check_input_mapping(root["input_mapping"]);
    check_app_config(root["app_config"]);

    throw_if_errors();

    for (std::size_t i = 0; i < staged_count; i++) {
        bool connection_storage_is_full = spec_count >= SimConfig::max_connection_number;

        if (connection_storage_is_full == true) {
            throw std::logic_error(SimConfig::capacity_message("[IO CONFIG] total connections across all apps",
                                                               "max_connection_number",
                                                               SimConfig::max_connection_number));
        }

        specs[spec_count] = staged_specs[i];
        spec_count++;
    }
}

void ConnectionYamlParser::check_top_level_keys(const YAML::Node& root) {
    for (YAML::const_iterator it = root.begin(); it != root.end(); ++it) {
        bool key_is_not_a_name = it->first.IsScalar() == false;

        if (key_is_not_a_name == true) {
            parse_report.record_error("the config file contains a top-level key that is not a plain name");
            continue;
        }

        std::string key = it->first.as<std::string>();

        bool key_is_a_section = key == "input_mapping" || key == "app_config";

        if (key_is_a_section == false) {
            parse_report.record_error(line_label(line_of(it->first)) + "unknown top-level key '" + key +
                                      "': the only sections are `input_mapping:` and `app_config:` "
                                      "(model parameters belong under `app_config:`)");
        }
    }
}

void ConnectionYamlParser::check_input_mapping(const YAML::Node& input_mapping) {
    std::string entry_form = "         - { input: <my_input>, from_app: <publisher_app>, from_port: <publisher_port> }";

    if (input_mapping.IsDefined() == false) {
        parse_report.record_error("missing required `input_mapping:` section. Use `input_mapping: []` for an app "
                                  "with no subscribed inputs");
    } else if (input_mapping.IsNull() == true) {
        parse_report.record_error(line_label(line_of(input_mapping)) + "`input_mapping:` has no value. Use "
                                  "`input_mapping: []` for an app with no subscribed inputs");
    } else if (input_mapping.IsSequence() == false) {
        parse_report.record_error(line_label(line_of(input_mapping)) + "`input_mapping:` must be a list, one "
                                  "entry per line:\n" + entry_form);
    } else {
        for (std::size_t i = 0; i < input_mapping.size(); i++) {
            parse_entry(input_mapping[i], i + 1);
        }
    }
}

void ConnectionYamlParser::check_app_config(const YAML::Node& app_config) {
    if (app_config.IsDefined() == false) {
        parse_report.record_error("missing required `app_config:` section. Use `app_config: {}` for an app "
                                  "with no parameters");
    } else if (app_config.IsNull() == true) {
        parse_report.record_error(line_label(line_of(app_config)) + "`app_config:` has no value. Use "
                                  "`app_config: {}` for an app with no parameters");
    } else if (app_config.IsMap() == false) {
        parse_report.record_error(line_label(line_of(app_config)) + "`app_config:` must be a map of "
                                  "parameter names to values");
    }
}

// An entry is only staged when all three fields are sound, so a broken entry
// can never become a half-formed connection
void ConnectionYamlParser::parse_entry(const YAML::Node& entry, std::size_t entry_number) {
    std::size_t line = line_of(entry);

    bool entry_is_not_a_map = entry.IsMap() == false;

    if (entry_is_not_a_map == true) {
        parse_report.record_error(line_label(line) + "entry " + std::to_string(entry_number) +
                                  " is not a set of named fields; write it as\n"
                                  "         - { input: <my_input>, from_app: <publisher_app>, from_port: <publisher_port> }");
        return;
    }

    check_map_keys(entry, line, entry_keys, 3);

    IoConfigSpec spec;
    spec.destination_app  = owner_name;
    spec.config_file_name = file_path + ":" + std::to_string(line);

    bool input_ok     = extract_name(entry, line, "input", spec.input);
    bool from_app_ok  = extract_name(entry, line, "from_app", spec.from_app);
    bool from_port_ok = extract_name(entry, line, "from_port", spec.from_port);

    bool entry_is_sound = input_ok == true && from_app_ok == true && from_port_ok == true;

    if (entry_is_sound == false) {
        return;
    }

    if (check_input_not_already_connected(spec.input, line) == false) {
        return;
    }

    bool staging_is_full = staged_count >= SimConfig::max_connection_number;

    if (staging_is_full == true) {
        throw std::logic_error(SimConfig::capacity_message("[IO CONFIG] " + owner_name + ": input_mapping entries",
                                                           "max_connection_number",
                                                           SimConfig::max_connection_number));
    }

    staged_specs[staged_count] = spec;
    staged_lines[staged_count] = line;
    staged_count++;
}

// The registry also catches this, but here the report can name both lines
bool ConnectionYamlParser::check_input_not_already_connected(const std::string& input_name, std::size_t line) {
    for (std::size_t i = 0; i < staged_count; i++) {
        bool input_already_connected = staged_specs[i].input == input_name;

        if (input_already_connected == true) {
            parse_report.record_error(line_label(line) + "input '" + input_name + "' is connected more than once "
                                      "(first connected at line " + std::to_string(staged_lines[i]) + ")");
            return false;
        }
    }

    return true;
}
