#ifndef YAML_PARSER_BASE_HPP
#define YAML_PARSER_BASE_HPP

#include <array>
#include <cstddef>
#include <string>

#include <yaml-cpp/yaml.h>

#include "io_registry.hpp"

// Shared machinery for the framework's config parsers (ConnectionYamlParser,
// AppManifestParser): bounded error collection with a single thrown report,
// line-numbered messages, and typed field extraction with specific errors for
// every failure mode (missing key, empty value, wrong shape, bad conversion,
// name-rule violation).
//
// Derived parsers record every problem they find and call throw_if_errors()
// once at the end, so the user sees the complete list in one run.
class YamlParserBase {
protected:
    // `report_tag` opens the thrown report (e.g. "[IO CONFIG]"), `owner_name`
    // says whose file this is (an app instance name or "app manifest")
    YamlParserBase(const std::string& report_tag, const std::string& owner_name, const std::string& path_to_file);

    // Loads the file, wrapping unreadable-file and YAML-syntax failures in an
    // IoWiringError that names the owner
    YAML::Node load_root();

    void record_error(const std::string& message);
    void throw_if_errors();
    bool has_errors() const { return total_error_count > 0; }

    // ---------------- Typed field extraction ----------------
    // Each records a specific error and returns false when the field is
    // missing, empty, non-scalar, fails conversion, or breaks the name rule
    bool extract_name(const YAML::Node& entry, std::size_t line, const char* key, std::string& value_out);
    bool extract_string(const YAML::Node& entry, std::size_t line, const char* key, std::string& value_out);
    bool extract_double(const YAML::Node& entry, std::size_t line, const char* key, double& value_out);
    bool extract_int(const YAML::Node& entry, std::size_t line, const char* key, int& value_out);
    bool extract_bool(const YAML::Node& entry, std::size_t line, const char* key, bool& value_out);

    // Rejects unknown keys (with a closest-match suggestion) and duplicate
    // keys within one map node, against the list of valid keys
    void check_map_keys(const YAML::Node& map_node, std::size_t line,
                        const char* const* valid_keys, std::size_t valid_key_count);

    static std::size_t line_of(const YAML::Node& node);
    static std::string line_label(std::size_t line);
    static std::string joined_map_keys(const YAML::Node& map_node);
    static std::string closest_key(const std::string& unknown_key,
                                   const char* const* valid_keys, std::size_t valid_key_count);

    std::string report_tag;
    std::string owner_name;
    std::string file_path;

private:
    // Shared checks for every extract_* variant: key present, has a value,
    // is a single scalar. Records the error and returns false otherwise
    bool extract_scalar_node(const YAML::Node& entry, std::size_t line, const char* key, YAML::Node& node_out);

    static constexpr std::size_t max_valid_key_number = 12;

    std::array<std::string, SimConfig::max_wiring_report_number> parse_errors;
    std::size_t stored_error_count = 0;
    std::size_t total_error_count  = 0;
};

#endif
