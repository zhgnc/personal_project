#include "io_report.hpp"

#include <sstream>

#include "port_matching.hpp"

std::string IoReport::source_name_of(const std::array<PortRecord, SimConfig::max_port_number>& ports,
                                     const PortRecord& record) {
    if (record.is_connected() == false) {
        return "";
    }

    const PortRecord& source = ports[record.source_numerical_index];

    return source.owner_app + "." + source.port_name;
}

std::string IoReport::build(const std::array<PortRecord, SimConfig::max_port_number>& ports,
                            std::size_t port_count,
                            const std::array<std::string, SimConfig::max_app_number>& app_names,
                            std::size_t app_count) {
    std::ostringstream out;

    out << "[IO REGISTRY] " << app_count << " app(s), " << port_count << " port(s)\n";

    for (std::size_t app_index = 0; app_index < app_count; app_index++) {
        const std::string& app_name = app_names[app_index];

        out << "\n" << app_name << "\n";

        // Inputs first, each with the output it resolved to
        for (std::size_t i = 0; i < port_count; i++) {
            const PortRecord& record = ports[i];

            if (record.owner_app != app_name || PortMatching::matches_direction(record, PortDirection::Input) == false) {
                continue;
            }

            out << "    <- " << record.port_name << "  [" << record.type_name << "]";

            if (record.is_connected()) {
                out << "  from " << source_name_of(ports, record);
            } else if (record.is_optional) {
                out << "  (optional, using default)";
            } else {
                out << "  (UNCONNECTED)";
            }

            out << "\n";
        }

        // Then the outputs this app publishes for everyone else
        for (std::size_t i = 0; i < port_count; i++) {
            const PortRecord& record = ports[i];

            if (record.owner_app != app_name || PortMatching::matches_direction(record, PortDirection::Output) == false) {
                continue;
            }

            out << "    -> " << record.port_name << "  [" << record.type_name << "]  "
                << port_kind_to_string(record.kind) << "\n";
        }
    }

    return out.str();
}
