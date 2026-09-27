// Implementation of the shared config-parsing machinery. See
// yaml_parser_base.hpp for what derived parsers get from it.

#include "yaml_parser_base.hpp"

#include <sstream>
#include <stdexcept>

#include "../../utilities/yaml_utilities.hpp"

YamlParserBase::YamlParserBase(const std::string& parser_report_tag, const std::string& parser_owner_name,
                               const std::string& path_to_file) {
    report_tag = parser_report_tag;
    owner_name = parser_owner_name;
    file_path  = path_to_file;
}

// Wraps yaml-cpp's file and syntax failures so a missing or malformed file
// reports as an IoWiringError naming the owner, like every other config
// problem, instead of a bare library message
YAML::Node YamlParserBase::load_root() {
    try {
        return load_yaml_file(file_path);
    } catch (const std::runtime_error& error) {
        throw IoWiringError(report_tag + " " + owner_name + ": cannot read config file\n  " + error.what());
    }
}

// Stores the first max_wiring_report_number messages but counts them all, so
// the thrown report can say how many were hidden
void YamlParserBase::record_error(const std::string& message) {
    if (stored_error_count < parse_errors.size()) {
        parse_errors[stored_error_count] = message;
        stored_error_count++;
    }

    total_error_count++;
}

// Called once at the end of a parse: succeeds silently when the file was
// clean, otherwise throws one exception listing every problem found
void YamlParserBase::throw_if_errors() {
    if (total_error_count == 0) {
        return;
    }

    std::ostringstream report;
    report << report_tag << " " << owner_name << " (" << file_path << "): " << total_error_count << " error(s):";

    if (total_error_count > stored_error_count) {
        report << "  (showing first " << stored_error_count << ")";
    }
    report << "\n";

    for (std::size_t i = 0; i < stored_error_count; i++) {
        report << "\n  " << (i + 1) << ". " << parse_errors[i] << "\n";
    }

    throw IoWiringError(report.str());
}

// ---------------- Typed field extraction ----------------

// Shared front half of every extract_*: the key must be present, have a
// value, and be a single scalar. Each failure gets its own message so the
// user is told which of the three went wrong
bool YamlParserBase::extract_scalar_node(const YAML::Node& entry, std::size_t line, const char* key,
                                         YAML::Node& node_out) {
    YAML::Node value = entry[key];

    if (value.IsDefined() == false) {
        record_error(line_label(line) + "entry is missing required key '" + key +
                     "' (found: " + joined_map_keys(entry) + ")");
        return false;
    }

    if (value.IsNull()) {
        record_error(line_label(line) + "key '" + std::string(key) + "' has no value");
        return false;
    }

    if (value.IsScalar() == false) {
        record_error(line_label(line) + "the value of '" + std::string(key) +
                     "' must be a single value, not a list or nested fields");
        return false;
    }

    node_out = value;
    return true;
}

// A string that must also satisfy the framework's name rule, used for
// anything that becomes an app or port identifier
bool YamlParserBase::extract_name(const YAML::Node& entry, std::size_t line, const char* key,
                                  std::string& value_out) {
    if (extract_string(entry, line, key, value_out) == false) {
        return false;
    }

    std::string violation = IoRegistry::io_name_violation(value_out);

    if (violation.empty() == false) {
        record_error(line_label(line) + "value '" + value_out + "' of key '" + std::string(key) +
                     "' is invalid: " + violation);
        return false;
    }

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

bool YamlParserBase::extract_double(const YAML::Node& entry, std::size_t line, const char* key,
                                    double& value_out) {
    YAML::Node value;

    if (extract_scalar_node(entry, line, key, value) == false) {
        return false;
    }

    try {
        value_out = value.as<double>();
    } catch (const YAML::Exception&) {
        record_error(line_label(line) + "the value of '" + std::string(key) + "' must be a number, got '" +
                     value.as<std::string>() + "'");
        return false;
    }

    return true;
}

bool YamlParserBase::extract_int(const YAML::Node& entry, std::size_t line, const char* key,
                                 int& value_out) {
    YAML::Node value;

    if (extract_scalar_node(entry, line, key, value) == false) {
        return false;
    }

    try {
        value_out = value.as<int>();
    } catch (const YAML::Exception&) {
        record_error(line_label(line) + "the value of '" + std::string(key) +
                     "' must be a whole number, got '" + value.as<std::string>() + "'");
        return false;
    }

    return true;
}

bool YamlParserBase::extract_bool(const YAML::Node& entry, std::size_t line, const char* key,
                                  bool& value_out) {
    YAML::Node value;

    if (extract_scalar_node(entry, line, key, value) == false) {
        return false;
    }

    try {
        value_out = value.as<bool>();
    } catch (const YAML::Exception&) {
        record_error(line_label(line) + "the value of '" + std::string(key) + "' must be true or false, got '" +
                     value.as<std::string>() + "'");
        return false;
    }

    return true;
}

// ---------------- Map key policing ----------------

// Rejects unknown and duplicated keys. Without this a misspelled or stray
// key would be silently ignored, which is the config failure mode that is
// hardest to notice — the file looks right and the setting does nothing
void YamlParserBase::check_map_keys(const YAML::Node& map_node, std::size_t line,
                                    const char* const* valid_keys, std::size_t valid_key_count) {
    if (valid_key_count > max_valid_key_number) {
        throw std::logic_error("YamlParserBase::check_map_keys(): valid_key_count exceeds max_valid_key_number");
    }

    std::array<std::size_t, max_valid_key_number> key_counts{};

    for (YAML::const_iterator it = map_node.begin(); it != map_node.end(); ++it) {
        if (it->first.IsScalar() == false) {
            record_error(line_label(line) + "entry contains a key that is not a plain name");
            continue;
        }

        std::string key = it->first.as<std::string>();
        bool known = false;

        for (std::size_t i = 0; i < valid_key_count; i++) {
            if (key == valid_keys[i]) {
                key_counts[i]++;
                known = true;
            }
        }

        if (known == false) {
            std::string valid_list;

            for (std::size_t i = 0; i < valid_key_count; i++) {
                valid_list += (i == 0 ? "" : ", ") + std::string(valid_keys[i]);
            }

            std::string suggestion = closest_key(key, valid_keys, valid_key_count);
            record_error(line_label(line) + "unknown key '" + key + "' (valid keys: " + valid_list + ")" +
                         (suggestion.empty() ? "" : "\n         did you mean '" + suggestion + "'?"));
        }
    }

    for (std::size_t i = 0; i < valid_key_count; i++) {
        if (key_counts[i] > 1) {
            record_error(line_label(line) + "key '" + std::string(valid_keys[i]) + "' appears " +
                         std::to_string(key_counts[i]) + " times in one entry");
        }
    }
}

// ---------------- Formatting helpers ----------------

// 1-based line number of a node for error messages, or 0 when yaml-cpp has
// no mark for it (which line_label renders as no prefix at all)
std::size_t YamlParserBase::line_of(const YAML::Node& node) {
    // yaml-cpp marks are 0-based and -1 when unavailable
    if (node.Mark().line < 0) {
        return 0;
    }

    return static_cast<std::size_t>(node.Mark().line) + 1;
}

std::string YamlParserBase::line_label(std::size_t line) {
    if (line == 0) {
        return "";
    }

    return "line " + std::to_string(line) + ": ";
}

std::string YamlParserBase::joined_map_keys(const YAML::Node& map_node) {
    std::string joined;

    for (YAML::const_iterator it = map_node.begin(); it != map_node.end(); ++it) {
        if (it->first.IsScalar() == false) {
            continue;
        }

        if (!joined.empty()) {
            joined += ", ";
        }
        joined += it->first.as<std::string>();
    }

    return joined.empty() ? "no keys" : joined;
}

std::string YamlParserBase::closest_key(const std::string& unknown_key,
                                        const char* const* valid_keys, std::size_t valid_key_count) {
    std::size_t best_distance = 3;  // small keysets, keep suggestions tight
    std::string best_match;

    for (std::size_t i = 0; i < valid_key_count; i++) {
        std::size_t distance = IoRegistry::edit_distance(unknown_key, valid_keys[i]);

        if (distance < best_distance) {
            best_distance = distance;
            best_match = valid_keys[i];
        }
    }

    return best_match;
}
