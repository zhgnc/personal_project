#ifndef PORT_MATCHING_HPP
#define PORT_MATCHING_HPP

// Checks names against the ports that actually exist.
//
// When a config file names something that was never declared, the framework
// reports what the user wrote and then lists every option they could have
// written instead. Listing the real options is deliberately preferred over
// guessing at the intended name: the list is always correct, needs no
// tuning, and tells the user what exists rather than one opinion about what
// they meant.
//
// Every function is static and is handed the port records it needs, so this
// class holds no state and needs no access to IoRegistry's internals.

#include <array>
#include <cstddef>
#include <string>
#include <typeindex>

#include "port_data.hpp"

class PortMatching {
public:
    static std::string name_violation(const std::string& name);                       // Checks name only includes letters, numbers, and `_`
    static bool matches_direction(const PortRecord& record, PortDirection direction); // Checks whether a port is on the requested side of its app

    static std::string list_port_names(const std::array<PortRecord, SimConfig::max_port_number>& ports,
                                       std::size_t port_count,
                                       const std::string& app_name,
                                       PortDirection direction);

    static std::string list_publishers_of_type(const std::array<PortRecord, SimConfig::max_port_number>& ports,
                                               std::size_t port_count,
                                               const std::type_index& type);

    static std::string list_app_names(const std::array<std::string, SimConfig::max_app_number>& app_names,
                                      std::size_t app_count);
};

#endif
