#ifndef IO_REGISTRY_HPP
#define IO_REGISTRY_HPP

// The IO registry is the heart of the sim framework's data passing.
//
// Apps declare their inputs and outputs in declare_io() by handing the
// registry a name and a reference to one of their own model's struct members.
// The registry stores one PortRecord per declaration, resolves the
// connections listed in the config yaml files against those records, and
// hands each app a pre-computed list of memcpys (AppIoInfo) that the frame
// loop executes. One declaration serves three consumers: data transport
// between apps, startup validation, and telemetry recording.
//
// The implementation is split across two files by phase:
//   io_registry.cpp             declarations, storage, lookup, runtime
//   io_registry_resolution.cpp  connecting and validating the topology

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <type_traits>

#include "port_data.hpp"
#include "port_type_name.hpp"
#include "port_matching.hpp"
#include "error_report.hpp"
#include "io_report.hpp"

// Owns all port declarations and connections for one simulation run. One
// IoRegistry exists per SimSingleRun and is populated after app cloning, so
// every port pointer targets that run's own app instances. Not thread-safe by
// design so each Monte Carlo run has its own private instance.
//
// Storage is fixed-size (limits in SimConfig), which makes this a large
// object: own it through a pointer, not as a local on a thread stack.
//
// Lifecycle is enforced, not documented-only: declarations are legal only
// between begin_declarations()/end_declarations() (i.e. inside declare_io()),
// and the topology freezes once resolve_and_validate() succeeds.
//
// Data transport guarantees:
//   - Subscribed inputs are value snapshots taken immediately before the
//     subscribing app steps. An app never sees data written later in the
//     same frame by lower-priority apps.
//   - If a publisher stops stepping, subscribers keep its last written
//     value, like a real vehicle data bus. last_write_usec is how a
//     subscriber can detect that.
//   - Wiring is resolved and validated before t=0: a sim that starts
//     stepping has no unconnected required inputs and no type mismatches.
class IoRegistry {
public:
    // Declaration API (called from an app's declare_io())
    template<typename T>
    void sub(const std::string& port_name, T& destination);

    template<typename T>
    void sub_optional(const std::string& port_name, T& destination, const T& default_value);

    template<typename T>
    void tlm(const std::string& port_name, T& source);
    
    template<typename T>
    void tlm_debug(const std::string& port_name, T& source);

    // Sim Framework API (called by SimSingleRun)
    void begin_declarations(const std::string& app_name);
    void end_declarations();
    void connect(const std::string& destination_app, 
                 const std::string& input,
                 const std::string& from_app,
                 const std::string& from_port,
                 const std::string& config_file_name);

    void resolve_and_validate();
    const AppIoInfo& plan_for(const std::string& app_name) const;
    void copy_inputs(const AppIoInfo& plan) const;
    void stamp_outputs(const AppIoInfo& plan, uint64_t sim_time_usec);

    // Internal use. Callers loop from 0 to the count and read one at a time
    std::size_t port_count() const { return port_record_count; }
    const PortRecord& port_at(std::size_t index) const { return port_records[index]; }

    // Non-fatal wiring observations collected during resolve_and_validate()
    const ErrorReport& report() const { return wiring_report; }

    // "app_name.port_name" of the output feeding this input, or an empty
    // string when the input is unconnected
    std::string source_name_of(const PortRecord& record) const {
        return IoReport::source_name_of(port_records, record);
    }

    // Human-readable dump of every app's ports, types, and resolved
    // sources — printed at startup so the connection names are never a guess
    std::string io_report() const {
        return IoReport::build(port_records, port_record_count, declared_app_names, declared_app_count);
    }

private:
    template<typename T>
    void add_record(const std::string& port_name, T& member, PortKind kind, bool optional_flag);

    // Phase guards: declaring outside declare_io(), or changing the topology
    // after it is resolved, is a programming error rather than a config error
    void require_declaration_open(const std::string& port_name) const;
    void require_not_resolved(const char* method_name) const;

    bool app_is_declared(const std::string& app_name) const;
    std::size_t app_index_of(const std::string& app_name) const;                                  // Declared app numerical index when not found
    std::size_t find_port_index(const std::string& app_name, const std::string& port_name) const; // Port record numerical index when not found


    void resolve_connection(const ConnectionRequest& request);
    bool check_destination_port(const ConnectionRequest& request, const std::string& context,
                                std::size_t& destination_index_out);
    bool check_source_port(const ConnectionRequest& request, const std::string& context,
                           std::size_t& source_index_out);
    bool check_payload_match(const ConnectionRequest& request, const std::string& context,
                             const PortRecord& source, const PortRecord& destination);
    void add_copy_binding(const ConnectionRequest& request, std::size_t source_index,
                          std::size_t destination_index);

    void check_unconnected_inputs();
    void build_output_stamp_lists();


    std::array<PortRecord, SimConfig::max_port_number> port_records;
    std::size_t port_record_count = 0;

    std::array<ConnectionRequest, SimConfig::max_connection_number> connection_requests;
    std::size_t connection_request_count = 0;

    std::array<std::string, SimConfig::max_app_number> declared_app_names;
    std::size_t declared_app_count = 0;

    std::array<AppIoInfo, SimConfig::max_app_number> app_plans;

    ErrorReport wiring_report;

    std::string app_open_for_declarations;  // empty when no declare_io() is in progress
    bool wiring_resolved = false;
};

#include "io_registry.tpp"

#endif
