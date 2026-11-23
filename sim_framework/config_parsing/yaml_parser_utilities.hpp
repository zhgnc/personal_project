#ifndef YAML_PARSER_UTILITIES_HPP
#define YAML_PARSER_UTILITIES_HPP

// Tools the framework's config file parsers are built from: line-numbered
// errors collected into one report, and field extraction that names exactly
// what is wrong with a field.

#include <cstddef>
#include <string>
#include <stdexcept>

#include <yaml-cpp/yaml.h>

#include "../messaging/error_report.hpp"
#include "../utilities/name_rules.hpp"
#include "../../utilities/yaml_utilities.hpp"

class YamlParserUtilities {
public:
    YamlParserUtilities() = default;

    // `report_tag` opens the thrown report (e.g. "[IO CONFIG]") and
    // `owner_name` says whose file this is
    YamlParserUtilities(const std::string& report_tag, const std::string& owner_name, const std::string& path_to_file);

    YAML::Node load_root();
    void record_error(const std::string& message);
    void throw_if_errors() const;

    // Record a specific error and return false when the field is missing,
    // empty, not a single value, or breaks the name rule
    bool extract_string(const YAML::Node& entry, std::size_t line, const char* key, std::string& value_out);
    bool extract_name(const YAML::Node& entry, std::size_t line, const char* key, std::string& value_out);

    // Records an error for every unknown key and every key that appears more
    // than once in one map entry
    void check_map_keys(const YAML::Node& map_node, std::size_t line,
                        const char* const* valid_keys, std::size_t valid_key_count);

    const std::string& owner() const { return owner_name; }
    const std::string& path() const { return file_path; }

    static std::size_t line_of(const YAML::Node& node);
    static std::string line_label(std::size_t line);

private:
    bool extract_single_value(const YAML::Node& entry, std::size_t line, const char* key, YAML::Node& node_out);
    static std::string joined_map_keys(const YAML::Node& map_node);

    std::string report_tag;
    std::string owner_name;
    std::string file_path;
    ErrorReport parse_report;
};

#endif
