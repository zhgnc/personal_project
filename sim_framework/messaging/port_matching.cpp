// Implementation of the name checks and option lists used in wiring errors.
// See port_matching.hpp for why the framework lists real options instead of
// guessing at the intended name.

#include "port_matching.hpp"

std::string PortMatching::name_violation(const std::string& name) {
    if (name.empty()) {
        return "name is empty";
    }

    for (char character : name) {
        bool valid = (character >= 'a' && character <= 'z') ||
                     (character >= '0' && character <= '9') ||
                     character == '_';

        if (valid == false) {
            std::string shown = (character == ' ') ? std::string("a space") : "'" + std::string(1, character) + "'";
            return "contains " + shown + "; names must use only lowercase letters, digits, and underscores";
        }
    }

    return "";
}

bool PortMatching::matches_direction(const PortRecord& record, PortDirection direction) {
    if (direction == PortDirection::Input) {
        return record.kind == PortKind::Subscribe;
    }

    return record.kind != PortKind::Subscribe;
}

std::string PortMatching::port_names_for(std::span<const PortRecord> ports,
                                         const std::string& app_name,
                                         PortDirection direction) {
    std::string joined;

    for (const PortRecord& record : ports) {
        if (record.owner_app == app_name && matches_direction(record, direction)) {
            joined += (joined.empty() ? "" : ", ") + record.port_name;
        }
    }

    return joined.empty() ? "(none)" : joined;
}

std::string PortMatching::publishers_of_type(std::span<const PortRecord> ports,
                                             const std::type_index& type) {
    std::string joined;

    for (const PortRecord& record : ports) {
        if (matches_direction(record, PortDirection::Output) && record.type == type) {
            joined += (joined.empty() ? "" : ", ") + record.owner_app + "." + record.port_name;
        }
    }

    return joined;
}

std::string PortMatching::app_names(std::span<const std::string> declared_app_names) {
    std::string joined;

    for (const std::string& name : declared_app_names) {
        joined += (joined.empty() ? "" : ", ") + name;
    }

    return joined.empty() ? "(none)" : joined;
}
