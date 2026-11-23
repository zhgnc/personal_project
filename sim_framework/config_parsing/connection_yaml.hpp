#ifndef CONNECTION_YAML_HPP
#define CONNECTION_YAML_HPP

// Parses one app's config file. The file has exactly two top-level sections,
// both required so nothing is ever left to a default:
//
//   input_mapping:      # one line per subscribed input, [] for none
//     - { input: delta_angles, from_app: gyro_app, from_port: measured_delta_angles }
//
//   app_config:         # model parameters for configure_model(), {} for none
//     angle_random_walk_1_sigma: 3.14e-7
//
// Every problem in the file is reported at once with line numbers.

#include <array>
#include <cstddef>
#include <string>

#include <yaml-cpp/yaml.h>

#include "yaml_parser_utilities.hpp"
#include "../messaging/port_data.hpp"

// One app's connections, in the order its config file lists them
struct AppConnections {
    std::array<IoConfigSpec, SimConfig::max_app_input_number> specs;
    std::size_t count = 0;
};

class ConnectionYamlParser {
public:
    ConnectionYamlParser(const std::string& owning_app_name, const std::string& path_to_config);

    // Returns this app's connections, or throws IoWiringError listing every
    // problem in the file
    AppConnections parse_input_mapping();

private:
    bool check_input_mapping_is_a_list(const YAML::Node& input_mapping);
    AppConnections read_connections(const YAML::Node& input_mapping);
    bool parse_entry(const YAML::Node& entry, std::size_t entry_number, IoConfigSpec& spec_out);
    void check_app_config(const YAML::Node& app_config);

    YamlParserUtilities yaml_utilities;

    static constexpr const char* section_keys[2] = {"input_mapping", "app_config"};
    static constexpr const char* entry_keys[3]   = {"input", "from_app", "from_port"};
};

#endif
