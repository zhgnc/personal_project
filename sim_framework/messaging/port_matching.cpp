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

std::string PortMatching::list_port_names(const std::array<PortRecord, SimConfig::max_port_number>& ports,
                                          std::size_t port_count,
                                          const std::string& app_name,
                                          PortDirection direction) {
    std::string joined;

    for (std::size_t i = 0; i < port_count; i++) {
        if (ports[i].owner_app == app_name && matches_direction(ports[i], direction)) {
            joined += (joined.empty() ? "" : ", ") + ports[i].port_name;
        }
    }

    return joined.empty() ? "(none)" : joined;
}

std::string PortMatching::list_publishers_of_type(const std::array<PortRecord, SimConfig::max_port_number>& ports,
                                                  std::size_t port_count,
                                                  const std::type_index& type) {
    std::string joined;

    for (std::size_t i = 0; i < port_count; i++) {
        if (matches_direction(ports[i], PortDirection::Output) && ports[i].type == type) {
            joined += (joined.empty() ? "" : ", ") + ports[i].owner_app + "." + ports[i].port_name;
        }
    }

    // Returns an empty string rather than "(none)" because the caller prints a
    // different sentence when nothing publishes the type
    return joined;
}

std::string PortMatching::list_app_names(const std::array<std::string, SimConfig::max_app_number>& app_names,
                                         std::size_t app_count) {
    std::string joined;

    for (std::size_t i = 0; i < app_count; i++) {
        joined += (joined.empty() ? "" : ", ") + app_names[i];
    }

    return joined.empty() ? "(none)" : joined;
}
