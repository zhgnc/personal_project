#ifndef CONNECTION_YAML_HPP
#define CONNECTION_YAML_HPP

#include <array>
#include <cstddef>
#include <string>

#include <yaml-cpp/yaml.h>

#include "io_registry.hpp"
#include "yaml_parser_base.hpp"

// Parses one app's config file. The file must contain exactly two top-level
// sections, both required so behavior is always spelled out:
//
//   input_mapping:      # one line per subscribed input ([] for none)
//     - { input: delta_angles,  from_app: gyro_app,  from_port: measured_delta_angles }
//
//   app_config:         # model parameters, handed to configure_model() ({} for none)
//     angle_random_walk_1_sigma: 3.14e-7
//
// The multi-line block spelling of an input_mapping entry is also accepted.
// Unknown top-level keys are rejected (with migration hints for the retired
// `connections`, `tlm_level`, and `tlm_rate_hz` keys), so a stray section can
// never be silently ignored.
//
// Robustness rules for input_mapping entries, checked before the wiring
// resolver ever runs: exactly the keys input / from_app / from_port (unknown
// keys get a "did you mean", missing keys are listed alongside what was
// found, duplicate keys are errors); every value a single non-empty
// lowercase_underscore name; the same input connected at most once per file.
//
// Every problem in the file is collected and reported in a single
// IoWiringError with file and line numbers; nothing is appended to the spec
// storage unless the whole file is clean.
class ConnectionYamlParser : public YamlParserBase {
public:
    ConnectionYamlParser(const std::string& owning_app_name, const std::string& path_to_config);

    // Validates the whole file's structure and appends this app's validated
    // connection specs to the given storage. Throws IoWiringError listing
    // every problem if any were found
    void parse_input_mapping(std::array<IoConnectionSpec, SimConfig::max_connection_number>& specs,
                             std::size_t& spec_count);

private:
    void check_top_level_keys(const YAML::Node& root);
    void parse_entry(const YAML::Node& entry, std::size_t entry_number);
    void check_duplicate_input(const std::string& input_name, std::size_t line);

    // Validated entries staged here until the whole file is known to be clean
    std::array<IoConnectionSpec, SimConfig::max_connection_number> staged_specs;
    std::size_t staged_count = 0;

    // Line each input was first connected on, for duplicate reports
    std::array<std::string, SimConfig::max_connection_number> seen_inputs;
    std::array<std::size_t, SimConfig::max_connection_number> seen_input_lines;
    std::size_t seen_input_count = 0;

    static constexpr const char* entry_keys[3] = {"input", "from_app", "from_port"};
};

#endif
