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
// The implementation is split by phase, one file each:
//   io_registry_declaration.cpp  building the port and app tables, and
//                                searching them by name
//   io_registry_validation.cpp   connecting the apps together, checking the
//                                result, and building the per-app plans
//   io_registry_runtime.cpp      everything the frame loop touches

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <type_traits>

#include "../utilities/name_rules.hpp"
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
// Call order, driven by the framework: add_app() followed by that app's
// declare_io() for every app, then connect() for every connection listed in
// the config files, then resolve_and_validate() once. Nothing guards that
// order at runtime because apps are only ever handed an IoDeclarations, which
// reaches the four declaration calls and nothing else.
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
    void add_app(const std::string& app_name); // Every port declared after this call belongs to this app
    void connect(const std::string& destination_app,
                 const std::string& input,
                 const std::string& from_app,
                 const std::string& from_port,
                 const std::string& config_file_name);

    void resolve_and_validate();
    const AppIoInfo& plan_for(const std::string& app_name) const;
    void copy_data(const AppIoInfo& plan) const;
    void time_stamp_outputs(const AppIoInfo& plan, uint64_t sim_time_usec);

    // Internal use. Callers loop from 0 to the count and read one at a time
    std::size_t port_count() const { return port_record_count; }
    const PortRecord& port_at(std::size_t index) const { return port_records[index]; }

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

    // Name checks. App names throw: a duplicate makes every port on the second
    // instance look like a redeclaration, so collecting would bury the one real
    // cause, and an invalid name corrupts the 'app_name.port_name' prefix that
    // every later message is printed with. Port names are independent of each
    // other, so they collect into wiring_report and report as one list
    void throw_if_app_name_is_invalid(const std::string& app_name) const;
    void throw_if_app_name_is_duplicate(const std::string& app_name) const;
    bool check_port_name_is_valid(const std::string& port_name);
    bool check_port_name_is_unique(const std::string& port_name);

    // Storage checks. Outgrowing a SimConfig limit is always fatal, and the
    // message names the constant to raise
    void throw_if_app_storage_full(const std::string& app_name) const;
    void throw_if_port_storage_full(const std::string& port_name) const;
    void throw_if_connection_storage_full(const ConnectionRequest& request) const;
    void throw_if_app_input_storage_full(const AppIoInfo& plan, const ConnectionRequest& request) const;
    void throw_if_app_output_storage_full(const AppIoInfo& plan, const std::string& owner_app) const;

    bool is_app_declared(const std::string& app_name) const;
    std::size_t get_app_index(const std::string& app_name) const;                                  // Declared app numerical index when not found
    std::size_t find_port_index(const std::string& app_name, const std::string& port_name) const; // Port record numerical index when not found


    void resolve_connection(const ConnectionRequest& request);
    void record_connection_error(const ConnectionRequest& request, const std::string& message);
    bool check_destination_port(const ConnectionRequest& request, std::size_t destination_index);
    bool check_source_port(const ConnectionRequest& request, std::size_t source_index);
    bool check_apps_are_different(const ConnectionRequest& request);
    bool check_types_match(const ConnectionRequest& request, std::size_t source_index,
                             std::size_t destination_index);
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

    // Owner of the ports being declared, both set by add_app()
    std::string declaring_app_name;
    std::size_t declaring_app_numerical_index = 0;
};

#include "io_registry.tpp"

#endif
