#include "connection_yaml.hpp"

#include <stdexcept>

ConnectionYamlParser::ConnectionYamlParser(const std::string& owning_app_name, const std::string& path_to_config)
    : yaml("[IO CONFIG]", owning_app_name, path_to_config) {}

void ConnectionYamlParser::parse_input_mapping(std::array<IoConfigSpec, SimConfig::max_connection_number>& specs,
                                               std::size_t& spec_count) {
    YAML::Node root = yaml.load_root();

    bool file_is_not_a_map = root.IsMap() == false;

    if (file_is_not_a_map == true) {
        yaml.record_error("the config file must be a map with the two sections `input_mapping:` and `app_config:`");
        yaml.throw_if_errors();
    }

    yaml.check_map_keys(root, yaml.line_of(root), section_keys, 2);

    YAML::Node input_mapping = root["input_mapping"];

    bool input_mapping_is_a_list = check_input_mapping_is_a_list(input_mapping);

    if (input_mapping_is_a_list == true) {
        add_connections(input_mapping, specs, spec_count);
    }

    check_app_config(root["app_config"]);

    yaml.throw_if_errors();
}

bool ConnectionYamlParser::check_input_mapping_is_a_list(const YAML::Node& input_mapping) {
    std::string entry_form = "         - { input: <my_input>, from_app: <publisher_app>, from_port: <publisher_port> }";

    if (input_mapping.IsDefined() == false) {
        yaml.record_error("missing required `input_mapping:` section. Use `input_mapping: []` for an app "
                          "with no subscribed inputs");
        return false;
    }

    if (input_mapping.IsNull() == true) {
        yaml.record_error(yaml.line_label(yaml.line_of(input_mapping)) + "`input_mapping:` has no value. Use "
                          "`input_mapping: []` for an app with no subscribed inputs");
        return false;
    }

    if (input_mapping.IsSequence() == false) {
        yaml.record_error(yaml.line_label(yaml.line_of(input_mapping)) + "`input_mapping:` must be a list, one "
                          "entry per line:\n" + entry_form);
        return false;
    }

    return true;
}

void ConnectionYamlParser::add_connections(const YAML::Node& input_mapping,
                                           std::array<IoConfigSpec, SimConfig::max_connection_number>& specs,
                                           std::size_t& spec_count) {
    for (std::size_t i = 0; i < input_mapping.size(); i++) {
        IoConfigSpec spec;

        bool entry_is_sound = parse_entry(input_mapping[i], i + 1, spec);

        if (entry_is_sound == false) {
            continue;
        }

        bool connection_storage_is_full = spec_count >= SimConfig::max_connection_number;

        if (connection_storage_is_full == true) {
            throw std::logic_error(SimConfig::capacity_message("[IO CONFIG] total connections across all apps",
                                                               "max_connection_number",
                                                               SimConfig::max_connection_number));
        }

        specs[spec_count] = spec;
        spec_count++;
    }
}

// Returns true only when all three fields are sound, so a broken entry never
// becomes a half-formed connection
bool ConnectionYamlParser::parse_entry(const YAML::Node& entry, std::size_t entry_number, IoConfigSpec& spec_out) {
    std::size_t line = yaml.line_of(entry);

    bool entry_is_not_a_map = entry.IsMap() == false;

    if (entry_is_not_a_map == true) {
        yaml.record_error(yaml.line_label(line) + "entry " + std::to_string(entry_number) +
                          " is not a set of named fields; write it as\n"
                          "         - { input: <my_input>, from_app: <publisher_app>, from_port: <publisher_port> }");
        return false;
    }

    yaml.check_map_keys(entry, line, entry_keys, 3);

    spec_out.destination_app  = yaml.owner();
    spec_out.config_file_name = yaml.path() + ":" + std::to_string(line);

    bool input_ok     = yaml.extract_name(entry, line, "input", spec_out.input);
    bool from_app_ok  = yaml.extract_name(entry, line, "from_app", spec_out.from_app);
    bool from_port_ok = yaml.extract_name(entry, line, "from_port", spec_out.from_port);

    bool entry_is_sound = input_ok == true && from_app_ok == true && from_port_ok == true;

    return entry_is_sound;
}

void ConnectionYamlParser::check_app_config(const YAML::Node& app_config) {
    if (app_config.IsDefined() == false) {
        yaml.record_error("missing required `app_config:` section. Use `app_config: {}` for an app "
                          "with no parameters");
    } else if (app_config.IsNull() == true) {
        yaml.record_error(yaml.line_label(yaml.line_of(app_config)) + "`app_config:` has no value. Use "
                          "`app_config: {}` for an app with no parameters");
    } else if (app_config.IsMap() == false) {
        yaml.record_error(yaml.line_label(yaml.line_of(app_config)) + "`app_config:` must be a map of "
                          "parameter names to values");
    }
}
