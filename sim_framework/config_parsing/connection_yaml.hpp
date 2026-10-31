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
// Every problem in the file is reported at once with line numbers, and no
// connection is kept unless the whole file is clean.

#include <array>
#include <cstddef>
#include <string>

#include <yaml-cpp/yaml.h>

#include "yaml_parser_base.hpp"
#include "../messaging/port_data.hpp"

class ConnectionYamlParser : public YamlParserBase {
public:
    ConnectionYamlParser(const std::string& owning_app_name, const std::string& path_to_config);

    // Appends this app's connections to `specs`, or throws IoWiringError
    // listing every problem in the file
    void parse_input_mapping(std::array<IoConfigSpec, SimConfig::max_connection_number>& specs,
                             std::size_t& spec_count);

private:
    void check_top_level_keys(const YAML::Node& root);
    void check_input_mapping(const YAML::Node& input_mapping);
    void check_app_config(const YAML::Node& app_config);
    void parse_entry(const YAML::Node& entry, std::size_t entry_number);
    bool check_input_not_already_connected(const std::string& input_name, std::size_t line);

    // Entries wait here until the whole file is known to be clean
    std::array<IoConfigSpec, SimConfig::max_connection_number> staged_specs;
    std::array<std::size_t, SimConfig::max_connection_number> staged_lines;
    std::size_t staged_count = 0;

    static constexpr const char* entry_keys[3] = {"input", "from_app", "from_port"};
};

#endif
