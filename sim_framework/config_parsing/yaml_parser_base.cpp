#include "yaml_parser_base.hpp"

#include <stdexcept>

#include "../../utilities/yaml_utilities.hpp"

YamlParserBase::YamlParserBase(const std::string& parser_report_tag,
                               const std::string& parser_owner_name,
                               const std::string& path_to_file) {
    report_tag = parser_report_tag;
    owner_name = parser_owner_name;
    file_path  = path_to_file;
}

// A missing or malformed file reports as an IoWiringError naming the owner,
// like every other config problem
YAML::Node YamlParserBase::load_root() {
    try {
        return load_yaml_file(file_path);
    } catch (const std::runtime_error& error) {
        throw IoWiringError(report_tag + " " + owner_name + ": cannot read config file\n  " + error.what());
    }
}

void YamlParserBase::throw_if_errors() const {
    parse_report.throw_if_errors(report_tag + " " + owner_name + " (" + file_path + "):");
}

bool YamlParserBase::extract_scalar_node(const YAML::Node& entry, std::size_t line, const char* key,
                                         YAML::Node& node_out) {
    YAML::Node value = entry[key];

    bool key_is_missing = value.IsDefined() == false;

    if (key_is_missing == true) {
        parse_report.record_error(line_label(line) + "entry is missing required key '" + key +
                                  "' (found: " + joined_map_keys(entry) + ")");
        return false;
    }

    bool key_has_no_value = value.IsNull();

    if (key_has_no_value == true) {
        parse_report.record_error(line_label(line) + "key '" + std::string(key) + "' has no value");
        return false;
    }

    bool value_is_not_single = value.IsScalar() == false;

    if (value_is_not_single == true) {
        parse_report.record_error(line_label(line) + "the value of '" + std::string(key) +
                                  "' must be a single value, not a list or nested fields");
        return false;
    }

    node_out = value;
    return true;
}

bool YamlParserBase::extract_string(const YAML::Node& entry, std::size_t line, const char* key,
                                    std::string& value_out) {
    YAML::Node value;

    if (extract_scalar_node(entry, line, key, value) == false) {
        return false;
    }

    value_out = value.as<std::string>();
    return true;
}

// For anything that becomes an app or port name
bool YamlParserBase::extract_name(const YAML::Node& entry, std::size_t line, const char* key,
                                  std::string& value_out) {
    if (extract_string(entry, line, key, value_out) == false) {
        return false;
    }

    NameViolation name_violation = NameRules::violation(value_out);

    if (name_violation != NameViolation::None) {
        parse_report.record_error(line_label(line) + "value '" + value_out + "' of key '" + std::string(key) +
                                  "' is invalid: " + NameRules::message(name_violation));
        return false;
    }

    return true;
}

bool YamlParserBase::extract_bool(const YAML::Node& entry, std::size_t line, const char* key, bool& value_out) {
    YAML::Node value;

    if (extract_scalar_node(entry, line, key, value) == false) {
        return false;
    }

    try {
        value_out = value.as<bool>();
    } catch (const YAML::Exception&) {
        parse_report.record_error(line_label(line) + "the value of '" + std::string(key) +
                                  "' must be true or false, got '" + value.as<std::string>() + "'");
        return false;
    }

    return true;
}

bool YamlParserBase::extract_int(const YAML::Node& entry, std::size_t line, const char* key, int& value_out) {
    YAML::Node value;

    if (extract_scalar_node(entry, line, key, value) == false) {
        return false;
    }

    try {
        value_out = value.as<int>();
    } catch (const YAML::Exception&) {
        parse_report.record_error(line_label(line) + "the value of '" + std::string(key) +
                                  "' must be a whole number, got '" + value.as<std::string>() + "'");
        return false;
    }

    return true;
}

bool YamlParserBase::extract_double(const YAML::Node& entry, std::size_t line, const char* key, double& value_out) {
    YAML::Node value;

    if (extract_scalar_node(entry, line, key, value) == false) {
        return false;
    }

    try {
        value_out = value.as<double>();
    } catch (const YAML::Exception&) {
        parse_report.record_error(line_label(line) + "the value of '" + std::string(key) +
                                  "' must be a number, got '" + value.as<std::string>() + "'");
        return false;
    }

    return true;
}

// Without this a misspelled or stray key would be silently ignored: the file
// looks right and the setting does nothing
void YamlParserBase::check_map_keys(const YAML::Node& map_node, std::size_t line,
                                    const char* const* valid_keys, std::size_t valid_key_count) {
    std::string valid_key_list;

    for (std::size_t i = 0; i < valid_key_count; i++) {
        valid_key_list += (i == 0 ? "" : ", ") + std::string(valid_keys[i]);
    }

    for (YAML::const_iterator it = map_node.begin(); it != map_node.end(); ++it) {
        bool key_is_not_a_name = it->first.IsScalar() == false;

        if (key_is_not_a_name == true) {
            parse_report.record_error(line_label(line) + "entry contains a key that is not a plain name");
            continue;
        }

        std::string key = it->first.as<std::string>();
        bool key_is_known = false;

        for (std::size_t i = 0; i < valid_key_count; i++) {
            key_is_known = key_is_known || key == valid_keys[i];
        }

        if (key_is_known == false) {
            parse_report.record_error(line_label(line) + "unknown key '" + key + "' (valid keys: " +
                                      valid_key_list + ")");
        }
    }

    for (std::size_t i = 0; i < valid_key_count; i++) {
        std::size_t occurrences = 0;

        for (YAML::const_iterator it = map_node.begin(); it != map_node.end(); ++it) {
            bool is_this_key = it->first.IsScalar() == true && it->first.as<std::string>() == valid_keys[i];
            occurrences += is_this_key ? 1 : 0;
        }

        bool key_is_repeated = occurrences > 1;

        if (key_is_repeated == true) {
            parse_report.record_error(line_label(line) + "key '" + std::string(valid_keys[i]) + "' appears " +
                                      std::to_string(occurrences) + " times in one entry");
        }
    }
}

// 1-based line number of a node, or 0 when yaml-cpp has no mark for it
std::size_t YamlParserBase::line_of(const YAML::Node& node) {
    bool node_has_no_mark = node.Mark().line < 0;

    if (node_has_no_mark == true) {
        return 0;
    }

    return static_cast<std::size_t>(node.Mark().line) + 1;
}

std::string YamlParserBase::line_label(std::size_t line) {
    bool line_is_unknown = line == 0;

    if (line_is_unknown == true) {
        return "";
    }

    return "line " + std::to_string(line) + ": ";
}

std::string YamlParserBase::joined_map_keys(const YAML::Node& map_node) {
    std::string joined;

    for (YAML::const_iterator it = map_node.begin(); it != map_node.end(); ++it) {
        bool key_is_a_name = it->first.IsScalar() == true;

        if (key_is_a_name == true) {
            joined += (joined.empty() == true ? "" : ", ") + it->first.as<std::string>();
        }
    }

    return joined.empty() == true ? "no keys" : joined;
}
