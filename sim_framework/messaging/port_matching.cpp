#include "port_matching.hpp"

std::string PortMatching::list_port_names(const std::array<PortRecord, SimConfig::max_port_number>& ports,
                                          std::size_t port_count,
                                          const std::string& app_name,
                                          PortDirection direction) {
    std::string joined;
    bool wants_inputs = direction == PortDirection::Input;

    for (std::size_t i = 0; i < port_count; i++) {
        if (ports[i].owner_app == app_name && ports[i].is_input() == wants_inputs) {
            joined += (joined.empty() ? "" : ", ") + ports[i].port_name;
        }
    }

    return joined.empty() ? "(none)" : joined;
}

std::string PortMatching::list_app_names(const std::array<std::string, SimConfig::max_app_number>& app_names,
                                         std::size_t app_count) {
    std::string joined;

    for (std::size_t i = 0; i < app_count; i++) {
        joined += (joined.empty() ? "" : ", ") + app_names[i];
    }

    return joined.empty() ? "(none)" : joined;
}
