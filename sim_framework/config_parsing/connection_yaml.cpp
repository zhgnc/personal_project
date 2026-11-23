#include "connection_yaml.hpp"

#include <stdexcept>

ConnectionYamlParser::ConnectionYamlParser(const std::string& owning_app_name, const std::string& path_to_config) {
    yaml_utilities = YamlParserUtilities("[IO CONFIG]", owning_app_name, path_to_config);
}

AppConnections ConnectionYamlParser::parse_input_mapping() {
    YAML::Node root = yaml_utilities.load_root();

    bool file_is_not_a_map = root.IsMap() == false; // A map is a file with only key values pairs

    if (file_is_not_a_map == true) {
        yaml_utilities.record_error("The config file must only contain key value pairs with the two sections `input_mapping:` and `app_config:`");
        yaml_utilities.throw_if_errors();
    }

    yaml_utilities.check_map_keys(root, yaml_utilities.line_of(root), section_keys, 2);

    YAML::Node input_mapping = root["input_mapping"];

    AppConnections app_connections;

    bool input_mapping_is_a_list = check_input_mapping_is_a_list(input_mapping);

    if (input_mapping_is_a_list == true) {
        app_connections = read_connections(input_mapping);
    }

    check_app_config(root["app_config"]);

    yaml_utilities.throw_if_errors();

    return app_connections;
}

bool ConnectionYamlParser::check_input_mapping_is_a_list(const YAML::Node& input_mapping) {
    std::string entry_form = "         - { input: <my_input>, from_app: <publisher_app>, from_port: <publisher_port> }";

    if (input_mapping.IsDefined() == false) {
        yaml_utilities.record_error("Missing required `input_mapping:` section. Use `input_mapping: []` for an app "
                                    "with no subscribed inputs");
        return false;
    }

    if (input_mapping.IsNull() == true) {
        yaml_utilities.record_error(yaml_utilities.line_label(yaml_utilities.line_of(input_mapping)) +
                                    "`input_mapping:` has no value. Use `input_mapping: []` for an app "
                                    "with no subscribed inputs");
        return false;
    }

    if (input_mapping.IsSequence() == false) {
        yaml_utilities.record_error(yaml_utilities.line_label(yaml_utilities.line_of(input_mapping)) +
                                    "`input_mapping:` must be a list, one entry per line:\n" + entry_form);
        return false;
    }

    return true;
}

AppConnections ConnectionYamlParser::read_connections(const YAML::Node& input_mapping) {
    AppConnections app_connections;

    for (std::size_t i = 0; i < input_mapping.size(); i++) {
        IoConfigSpec spec;

        bool valid_config = parse_entry(input_mapping[i], i + 1, spec);

        if (valid_config == false) {
            continue;
        }

        bool app_input_storage_is_full = app_connections.count >= SimConfig::max_app_input_number;

        if (app_input_storage_is_full == true) {
            throw std::logic_error(SimConfig::capacity_message("[IO CONFIG] " + yaml_utilities.owner() +
                                                               ": input_mapping entries",
                                                               "max_app_input_number",
                                                               SimConfig::max_app_input_number));
        }

        app_connections.specs[app_connections.count] = spec;
        app_connections.count++;
    }

    return app_connections;
}

// Returns true only when all three fields are sound, so a broken entry never
// becomes a half-formed connection
bool ConnectionYamlParser::parse_entry(const YAML::Node& entry, std::size_t entry_number, IoConfigSpec& spec_out) {
    std::size_t line = yaml_utilities.line_of(entry);

    bool entry_is_not_a_map = entry.IsMap() == false;

    if (entry_is_not_a_map == true) {
        yaml_utilities.record_error(yaml_utilities.line_label(line) + "Entry " + std::to_string(entry_number) +
                                    " is not a set of named fields; write it as\n"
                                    "         - { input: <my_input>, from_app: <publisher_app>, from_port: <publisher_port> }");
        return false;
    }

    yaml_utilities.check_map_keys(entry, line, entry_keys, 3);

    spec_out.destination_app  = yaml_utilities.owner();
    spec_out.config_file_name = yaml_utilities.path() + ":" + std::to_string(line);

    bool input_ok     = yaml_utilities.extract_name(entry, line, "input", spec_out.input);
    bool from_app_ok  = yaml_utilities.extract_name(entry, line, "from_app", spec_out.from_app);
    bool from_port_ok = yaml_utilities.extract_name(entry, line, "from_port", spec_out.from_port);

    bool entry_is_sound = input_ok == true && from_app_ok == true && from_port_ok == true;

    return entry_is_sound;
}

void ConnectionYamlParser::check_app_config(const YAML::Node& app_config) {
    if (app_config.IsDefined() == false) {
        yaml_utilities.record_error("Missing required `app_config:` section. Use `app_config: {}` for an app with no parameters");
    } else if (app_config.IsNull() == true) {
        yaml_utilities.record_error(yaml_utilities.line_label(yaml_utilities.line_of(app_config)) +
                                    "`app_config:` has no value. Use `app_config: {}` for an app with no parameters");
    } else if (app_config.IsMap() == false) {
        yaml_utilities.record_error(yaml_utilities.line_label(yaml_utilities.line_of(app_config)) +
                                    "`app_config:` must be a map of parameter names to values");
    }
}
