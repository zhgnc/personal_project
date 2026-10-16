#ifndef IO_REPORT_HPP
#define IO_REPORT_HPP

// Builds the startup dump of every app's ports and where their data comes
// from. This is what makes writing an `input_mapping:` block copy-and-paste
// rather than guesswork: the exact names are printed every run.
//
// Separate from IoRegistry because it only reads port records and app names.
// It is handed what it needs and holds no state.

#include <array>
#include <cstddef>
#include <string>

#include "port_data.hpp"

class IoReport {
public:
    static std::string build(const std::array<PortRecord, SimConfig::max_port_number>& ports,
                             std::size_t port_count,
                             const std::array<std::string, SimConfig::max_app_number>& app_names,
                             std::size_t app_count);

    // "app_name.port_name" of the output feeding this input, or an empty
    // string when the input is not connected
    static std::string source_name_of(const std::array<PortRecord, SimConfig::max_port_number>& ports,
                                      const PortRecord& record);
};

#endif
