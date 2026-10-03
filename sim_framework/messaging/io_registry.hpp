#ifndef IO_REGISTRY_HPP
#define IO_REGISTRY_HPP

// The IO registry is the heart of the sim framework's data passing.
//
// Apps declare their inputs and outputs in declare_io() by handing the
// registry a name and a reference to one of their own model's struct members.
// The registry stores one PortRecord per declaration, resolves the
// connections listed in the config yaml files against those records, and
// hands each app a pre-computed list of memcpys (AppIoPlan) that the frame
// loop executes. One declaration serves three consumers: data transport
// between apps, startup validation, and telemetry recording.
//
// The implementation is split across two files by phase:
//   io_registry.cpp             declarations, storage, lookup, runtime
//   io_registry_resolution.cpp  connecting and validating the topology

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <span>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <type_traits>

#include "port_data.hpp"
#include "port_matching.hpp"

// Owns all port declarations and connections for one simulation run. One
// IoRegistry exists per SimSingleRun and is populated after app cloning, so
// every port pointer targets that run's own app instances. Not thread-safe by
// design: each Monte Carlo run has its own private instance.
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
//     subscribing app steps; an app never sees data written later in the
//     same frame by lower-priority apps.
//   - If a publisher stops stepping, subscribers keep its last written
//     value, like a real vehicle data bus. last_write_usec is how a
//     subscriber can detect that.
//   - Wiring is resolved and validated before t=0: a sim that starts
//     stepping has no unconnected required inputs and no type mismatches.
class IoRegistry {
public:
    // ---------------- Declaration API (called from an app's declare_io()) ----------------

    // Declares an input. `destination` is a member of this app's model that
    // the framework fills from the connected output before every step
    template<typename T>
    void sub(const std::string& port_name, T& destination);

    // Declares an input that does not have to be connected. `default_value`
    // is written to `destination` immediately, and stands for the whole run
    // if no config file connects this input
    template<typename T>
    void sub_optional(const std::string& port_name, T& destination, const T& default_value);

    // Declares a required-telemetry output: readable by other apps, and
    // recorded whenever the app's tlm_level is `required` or `debug`
    template<typename T>
    void tlm_req(const std::string& port_name, T& source);

    // Declares a debug-telemetry output: readable by other apps exactly like
    // tlm_req, but only recorded when the app's tlm_level is `debug`
    template<typename T>
    void tlm_debug(const std::string& port_name, T& source);

    // ---------------- Framework API (called by SimSingleRun) ----------------

    // Opens/closes the declaration phase for one app. Every port declared in
    // between belongs to `app_name`
    void begin_declarations(const std::string& app_name);
    void end_declarations();

    // Records one requested connection. Requests may arrive in any order and
    // are all checked later by resolve_and_validate()
    void connect(const std::string& destination_app, const std::string& input,
                 const std::string& from_app, const std::string& from_port,
                 const std::string& origin);

    // Checks every connection request and the completeness of every app's
    // inputs, then builds the per-app copy plans. Collects all problems and
    // throws one IoWiringError listing them; on success the topology freezes
    void resolve_and_validate();

    // The resolved plan for one app. Valid only after resolve_and_validate()
    const AppIoPlan& plan_for(const std::string& app_name) const;

    // Copies every subscribed input for one app from its source. Called
    // immediately before the app steps
    void copy_inputs(const AppIoPlan& plan) const;

    // Marks every output of one app as written at `sim_time_usec`. Called
    // immediately after the app steps, so subscribers can measure data age
    void stamp_outputs(const AppIoPlan& plan, uint64_t sim_time_usec);

    // ---------------- Introspection ----------------

    // All declared ports, in declaration order. The recorder and the app-side
    // IO queries walk this
    std::span<const PortRecord> ports() const {
        return std::span<const PortRecord>(port_records.data(), port_record_count);
    }

    // Non-fatal wiring observations collected during resolve_and_validate()
    std::span<const std::string> warnings() const {
        return std::span<const std::string>(wiring_warnings.data(), wiring_warning_count);
    }

    // True when the warning array filled up and some warnings were dropped
    bool warnings_truncated() const { return total_warning_count > wiring_warning_count; }
    std::size_t total_warnings() const { return total_warning_count; }

    // "app_name.port_name" of the output feeding this input, or an empty
    // string when the input is unconnected
    std::string source_name_of(const PortRecord& record) const;

    // Human-readable dump of every app's ports, types, and resolved
    // sources — printed at startup so the connection names are never a guess
    std::string io_report() const;

private:
    // Shared body of sub/sub_optional/tlm_req/tlm_debug: validates the name,
    // captures the member's address and type, and appends a PortRecord
    template<typename T>
    void add_record(const std::string& port_name, T& member, PortKind kind, bool optional_flag);

    // Demangled type name (e.g. "quat<double>") used in error messages
    template<typename T>
    static std::string pretty_type_name();

    // Phase guards: declaring outside declare_io(), or changing the topology
    // after it is resolved, is a programming error rather than a config error
    void require_declaration_open(const std::string& port_name) const;
    void require_not_resolved(const char* method_name) const;

    // Bounded collection: every problem is counted, the first
    // max_wiring_report_number are stored for the report
    void record_error(const std::string& message);
    void record_warning(const std::string& message);

    // ---- Lookup ----
    bool        app_is_declared(const std::string& app_name) const;
    std::size_t app_index_of(const std::string& app_name) const;    // declared_app_count when not found
    std::size_t find_port_index(const std::string& app_name, const std::string& port_name) const;  // port_record_count when not found

    // Declared app names as a span, so PortMatching can list them
    std::span<const std::string> declared_apps() const {
        return std::span<const std::string>(declared_app_names.data(), declared_app_count);
    }

    // ---- resolve_and_validate() stages ----
    // resolve_connection runs the checks below in order and stops at the
    // first failure, because later checks would only repeat the same cause
    void resolve_connection(const ConnectionRequest& request);

    // Each returns false and records the error when its side does not check
    // out. The index outputs are only meaningful when the call returns true
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

    // ---------------- Port and connection storage ----------------
    std::array<PortRecord, SimConfig::max_port_number> port_records;
    std::size_t port_record_count = 0;

    std::array<ConnectionRequest, SimConfig::max_connection_number> connection_requests;
    std::size_t connection_request_count = 0;

    std::array<std::string, SimConfig::max_app_number> declared_app_names;
    std::size_t declared_app_count = 0;

    // Parallel to declared_app_names: app_plans[i] belongs to declared_app_names[i]
    std::array<AppIoPlan, SimConfig::max_app_number> app_plans;

    // ---------------- Validation results ----------------
    // The total counts can exceed the stored counts when an array fills; the
    // report says how many are shown
    std::array<std::string, SimConfig::max_wiring_report_number> wiring_errors;
    std::size_t stored_error_count = 0;
    std::size_t total_error_count  = 0;

    std::array<std::string, SimConfig::max_wiring_report_number> wiring_warnings;
    std::size_t wiring_warning_count = 0;
    std::size_t total_warning_count  = 0;

    std::string app_open_for_declarations;  // empty when no declare_io() is in progress
    bool wiring_resolved = false;
};

#include "io_registry.tpp"

#endif
