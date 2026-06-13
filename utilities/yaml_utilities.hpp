#ifndef YAML_UTILITIES_HPP
#define YAML_UTILITIES_HPP

#include <yaml-cpp/yaml.h>
#include <string>
#include <sstream>
#include <stdexcept>

inline YAML::Node load_yaml_file(const std::string& file_path) {
    try {
        return YAML::LoadFile(file_path);
    } 
    catch (const YAML::BadFile& e) {
        throw std::runtime_error(
            "Unable to open YAML file because of a bad file path or no read permissions: " + file_path +
            "\nDetails from yaml-cpp: " + e.what()
        );
    } 
    catch (const YAML::ParserException& e) {
        throw std::runtime_error(
            "Unable to open YAML file because of syntax errors: " + file_path +
            "\nDetails from yaml-cpp: " + e.what()
        );
    } 
    catch (const std::exception& e) {
        throw std::runtime_error(
            "Unable to open YAML file because of an unknown problem: " + file_path +
            "\nDetails from yaml-cpp: " + e.what()
        );
    }
}

template<typename T>
T get_yaml_value(const YAML::Node& node, const std::string& key) {
    try {
        return node[key].as<T>();
    } 
    catch (const YAML::InvalidNode& e) {
        throw std::runtime_error(
            "Key \"" + key + "\" is invalid (typo, missing, or null value)"
            + "\nDetails from yaml-cpp: " + e.what()
        );
    } 
    catch (const YAML::TypedBadConversion<T>& e) {
        throw std::runtime_error(
            "Failed to convert the value of key \"" + key + "\"  to requested type"
            + "\nDetails from yaml-cpp: " + e.what()
        );
    } 
    catch (const YAML::RepresentationException& e) {
        throw std::runtime_error(
            "Failed to interpret value of key \"" + key + "\" because the requested data type is not supported by yaml-cpp"
            + "\nDetails from yaml-cpp: " + e.what()
        );
    }
    catch (const YAML::Exception& e) {
        throw std::runtime_error(
            "Unknown YAML error while reading key \"" + key + "\""
            + "\nDetails from yaml-cpp: " + e.what()
        );
    }
}

#endif
