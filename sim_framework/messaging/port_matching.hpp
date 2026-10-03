#ifndef PORT_MATCHING_HPP
#define PORT_MATCHING_HPP

// Checking names against the ports that actually exist.
//
// When a config file names something that was never declared, the framework
// reports what the user wrote and then lists every option they could have
// written instead. Listing the real options is deliberately preferred over
// guessing at the intended name: the list is always correct, needs no
// tuning, and tells the user what exists rather than one opinion about what
// they meant.
//
// Every function is static and takes the port records as a span, so this
// class holds no state and needs no access to IoRegistry's internals.

#include <span>
#include <string>
#include <typeindex>

#include "port_data.hpp"

class PortMatching {
public:
    // App instance and port names may use only lowercase letters, digits, and
    // underscores. Returns an empty string when `name` complies, otherwise a
    // description of the violation for an error message
    static std::string name_violation(const std::string& name);

    // True when a port is on the requested side of its app
    static bool matches_direction(const PortRecord& record, PortDirection direction);

    // ---------------- "here is what you could have written" lists ----------------

    // Comma-separated list of one app's input or output names, or "(none)"
    static std::string port_names_for(std::span<const PortRecord> ports,
                                      const std::string& app_name,
                                      PortDirection direction);

    // Every output anywhere in the sim carrying `type`, as "app.port" entries.
    // Returns an empty string (not "(none)") when there are none, because the
    // caller prints a different sentence in that case
    static std::string publishers_of_type(std::span<const PortRecord> ports,
                                          const std::type_index& type);

    // Comma-separated list of every declared app instance name, or "(none)"
    static std::string app_names(std::span<const std::string> declared_app_names);
};

#endif
