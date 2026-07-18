#ifndef CONNECTION_YAML_HPP
#define CONNECTION_YAML_HPP

#include <array>
#include <cstddef>
#include <string>

#include <yaml-cpp/yaml.h>

#include "io_registry.hpp"

// Parses the optional `connections:` block of one app's config file into
// IoConnectionSpec entries. Expected format, one line per connection with
// every field named:
//
//   connections:
//     - { input: delta_angles,  from_app: gyro_app,  from_port: measured_delta_angles }
//
// The multi-line block spelling of the same map is also accepted. A missing
// `connections:` key is legal (the app has no inputs to wire, or only
// optional ones).
//
// Robustness rules enforced here, before the wiring resolver ever runs:
//   - the block must be a list; the old compact `input: app.port` map form
//     is rejected with a migration hint
//   - each entry must contain exactly the keys input / from_app / from_port:
//     unknown keys get a "did you mean" suggestion, missing keys are listed
//     alongside the keys that were found, duplicate keys are errors
//   - every value must be a single non-empty name made of lowercase letters,
//     digits, and underscores (IoRegistry::io_name_violation)
//   - the same input may not be connected twice in one file
//
// Every problem in the file is collected and reported in a single
// IoWiringError with file and line numbers; nothing is appended to the spec
// storage unless the whole file is clean.
class ConnectionYamlParser {
public:
    ConnectionYamlParser(const std::string& owning_app_name, const std::string& path_to_config);

    // Appends this app's validated connection specs to the given storage.
    // Throws IoWiringError listing every problem if any were found
    void parse_connections(std::array<IoConnectionSpec, SimConfig::max_connection_number>& specs,
                           std::size_t& spec_count);

    // Reads the optional `tlm_rate_hz` (must be a positive number; defaults
    // to `default_rate_hz`) and `tlm_level` (off / required / debug; defaults
    // to debug) keys. Throws IoWiringError on invalid values
    AppTlmSettings parse_tlm_settings(double default_rate_hz);

private:
    void parse_entry(const YAML::Node& entry, std::size_t entry_number);
    void check_entry_keys(const YAML::Node& entry, std::size_t line);
    bool extract_field(const YAML::Node& entry, std::size_t line, const char* key, std::string& value_out);
    void check_duplicate_input(const std::string& input_name, std::size_t line);

    void record_error(const std::string& message);
    void throw_if_errors();

    static std::size_t line_of(const YAML::Node& node);
    static std::string line_label(std::size_t line);
    static std::string joined_entry_keys(const YAML::Node& entry);
    static std::string suggest_entry_key(const std::string& unknown_key);

    std::string app_name;
    std::string config_path;

    // Validated entries staged here until the whole file is known to be clean
    std::array<IoConnectionSpec, SimConfig::max_connection_number> staged_specs;
    std::size_t staged_count = 0;

    // Line each input was first connected on, for duplicate reports
    std::array<std::string, SimConfig::max_connection_number> seen_inputs;
    std::array<std::size_t, SimConfig::max_connection_number> seen_input_lines;
    std::size_t seen_input_count = 0;

    std::array<std::string, SimConfig::max_wiring_report_number> parse_errors;
    std::size_t stored_error_count = 0;
    std::size_t total_error_count = 0;

    static constexpr const char* required_keys[3] = {"input", "from_app", "from_port"};
};

#endif
