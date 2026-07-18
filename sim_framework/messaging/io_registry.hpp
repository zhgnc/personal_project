#ifndef IO_REGISTRY_HPP
#define IO_REGISTRY_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <span>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <type_traits>

#include "../sim_core/sim_config.hpp"

// Subscribe ports are inputs filled by the framework before each step. TlmReq
// and DebugTlm ports are outputs: both are available to every other app as
// connection sources, the kind only controls whether the recorder stores them
// at the app's configured telemetry level.
enum class PortKind : uint8_t {
    Subscribe = 0,
    TlmReq    = 1,
    DebugTlm  = 2,
};

// Units are optional metadata checked at wiring time: connecting two ports
// that both declare units is an error if the units differ. There is no
// automatic conversion.
enum class Units : uint8_t {
    Unspecified = 0,
    Dimensionless,
    Seconds,
    Microseconds,
    Hertz,
    Meters,
    MetersPerSec,
    MetersPerSec2,
    Radians,
    RadiansPerSec,
    RadiansPerSec2,
    Kilograms,
    Newtons,
    NewtonMeters,
    Teslas,
    PartsPerMillion,
};

// Per-app recording level: Off records nothing, Required records tlm_req
// ports only, Debug records tlm_req and debug_tlm ports. Data passing is
// unaffected — unrecorded ports are still available to other apps
enum class TlmLevel : uint8_t {
    Off      = 0,
    Required = 1,
    Debug    = 2,
};

inline std::string tlm_level_to_string(TlmLevel level) {
    switch (level) {
        case TlmLevel::Off:      return "off";
        case TlmLevel::Required: return "required";
        case TlmLevel::Debug:    return "debug";
        default:                 return "Error Reporting TlmLevel Enum!";
    }
}

inline std::string port_kind_to_string(PortKind kind) {
    switch (kind) {
        case PortKind::Subscribe: return "subscribe";
        case PortKind::TlmReq:    return "tlm_req";
        case PortKind::DebugTlm:  return "debug_tlm";
        default:                  return "Error Reporting PortKind Enum!";
    }
}

inline std::string units_to_string(Units units) {
    switch (units) {
        case Units::Unspecified:     return "unspecified";
        case Units::Dimensionless:   return "-";
        case Units::Seconds:         return "sec";
        case Units::Microseconds:    return "usec";
        case Units::Hertz:           return "Hz";
        case Units::Meters:          return "m";
        case Units::MetersPerSec:    return "m/s";
        case Units::MetersPerSec2:   return "m/s^2";
        case Units::Radians:         return "rad";
        case Units::RadiansPerSec:   return "rad/s";
        case Units::RadiansPerSec2:  return "rad/s^2";
        case Units::Kilograms:       return "kg";
        case Units::Newtons:         return "N";
        case Units::NewtonMeters:    return "N*m";
        case Units::Teslas:          return "T";
        case Units::PartsPerMillion: return "ppm";
        default:                     return "Error Reporting Units Enum!";
    }
}

// One record per declared port. This is the single source of truth that
// wiring validation, the runtime copy plan, telemetry recording, and the
// introspection report are all built from. Every member has a default value
// so unused slots in the registry's fixed-size storage are well defined.
struct PortRecord {
    static constexpr uint64_t never_written = std::numeric_limits<uint64_t>::max();

    std::string owner_app;
    std::string port_name;

    void*           data_ptr   = nullptr;
    std::size_t     size_bytes = 0;
    std::type_index type       = std::type_index(typeid(void));
    std::string     type_name;

    PortKind kind  = PortKind::Subscribe;
    Units    units = Units::Unspecified;

    bool        is_optional          = false;  // Subscribe ports only: unconnected is legal, default value is used
    bool        is_connected         = false;  // Subscribe ports only: set by resolve_and_validate()
    bool        connection_attempted = false;  // Subscribe ports only: a connect() targeted this port, even if it failed
    std::string connected_source;              // Subscribe ports only: "app_name.port_name" once connected

    uint64_t last_write_usec = never_written;  // Output ports only: sim time of owner's most recent step
};

// One pre-resolved memcpy from a publisher's member into a subscriber's member
struct CopyBinding {
    const void* source_ptr      = nullptr;
    void*       destination_ptr = nullptr;
    std::size_t size_bytes      = 0;
};

// Per-app runtime view handed to the frame loop once after wiring resolves:
// input copies to execute before the app steps and the indices of the app's
// output records to time-stamp after it steps
struct AppIoPlan {
    std::array<CopyBinding, SimConfig::max_app_input_number> input_copies;
    std::size_t input_copy_count = 0;

    std::array<std::size_t, SimConfig::max_app_output_number> output_record_indices;
    std::size_t output_record_count = 0;
};

// A connection with every field explicit, as stored by SimManager and
// replayed into every run's own registry. Produced by the YAML connection
// parser or by splitting the run-file "app_name.port_name" convenience form
struct IoConnectionSpec {
    std::string destination_app;
    std::string input;
    std::string from_app;
    std::string from_port;
    std::string origin = "code";
};

// Per-app telemetry settings read from the app's config yaml (`tlm_rate_hz`,
// `tlm_level`), applied by the recorder when it registers HDF5 datasets
struct AppTlmSettings {
    std::string app_name;
    double rate_hz = 0.0;
    TlmLevel level = TlmLevel::Debug;
};

// A requested connection recorded by connect() and checked during
// resolve_and_validate(). `origin` says where the request came from (a config
// file and line, or "code") so error messages can point at the right place.
struct ConnectionRequest {
    std::string destination_app;
    std::string destination_port;
    std::string source_app;
    std::string source_port;
    std::string origin;
};

// Thrown by resolve_and_validate() with a report listing every wiring problem
// found, not just the first one
class IoWiringError : public std::runtime_error {
public:
    explicit IoWiringError(const std::string& report) : std::runtime_error(report) {}
};

// Owns all port declarations and connections for one simulation run. One
// IoRegistry exists per SimSingleRun and is populated after app cloning, so
// every port pointer targets that run's own app instances. Not thread-safe by
// design: each Monte Carlo run has its own private instance.
//
// Storage is fixed-size (limits in SimConfig), which makes this a large
// object: own it as a class member of the run object, not a transient local
// on a thread stack.
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
//     value, like a real vehicle data bus.
//   - Wiring is resolved and validated before t=0: a sim that starts
//     stepping has no unconnected required inputs, no type mismatches, and
//     no unit mismatches.
class IoRegistry {
public:
    // ---------------- Declaration API (called from an app's declare_io()) ----------------
    template<typename T>
    void subscribe(const std::string& port_name, T& destination, Units units = Units::Unspecified);

    // The default is written to `destination` at declaration time; if the
    // input is never connected the app simply runs with the default
    template<typename T>
    void subscribe_optional(const std::string& port_name, T& destination, const T& default_value, Units units = Units::Unspecified);

    template<typename T>
    void tlm_req(const std::string& port_name, T& source, Units units = Units::Unspecified);

    template<typename T>
    void debug_tlm(const std::string& port_name, T& source, Units units = Units::Unspecified);

    // ---------------- Framework API (called by SimSingleRun) ----------------
    void begin_declarations(const std::string& app_name);
    void end_declarations();

    // Both specs are fully qualified "app_name.port_name" strings. Requests
    // are recorded in any order and checked during resolve_and_validate()
    void connect(const std::string& destination, const std::string& source, const std::string& origin = "code");

    // Explicit-field form used by the YAML connection parser and spec replay;
    // involves no "app.port" string splitting
    void connect(const std::string& destination_app, const std::string& input,
                 const std::string& from_app, const std::string& from_port,
                 const std::string& origin);

    // Resolves every connection request and validates the whole topology,
    // collecting all errors before throwing IoWiringError
    void resolve_and_validate();

    const AppIoPlan& plan_for(const std::string& app_name) const;
    void copy_inputs(const AppIoPlan& plan) const;
    void stamp_outputs(const AppIoPlan& plan, uint64_t sim_time_usec);

    // ---------------- Introspection ----------------
    bool resolved() const { return wiring_resolved; }

    std::span<const PortRecord> ports() const {
        return std::span<const PortRecord>(port_records.data(), port_record_count);
    }

    std::span<const std::string> warnings() const {
        return std::span<const std::string>(wiring_warnings.data(), wiring_warning_count);
    }

    // Human-readable dump of every app's ports, types, units, and resolved
    // sources (the --print-io output)
    std::string io_report() const;

    // ---------------- Shared name utilities ----------------
    // App instance names and port names must use only lowercase letters,
    // digits, and underscores. Returns an empty string when `name` complies,
    // otherwise a description of the violation for error messages
    static std::string io_name_violation(const std::string& name);

    // Splits "app_name.port_name" on the first '.'; returns false when the
    // spec is not of that form
    static bool split_port_spec(const std::string& spec, std::string& app_name, std::string& port_name);

    // Levenshtein distance, shared by every "did you mean" suggestion
    static std::size_t edit_distance(const std::string& word_a, const std::string& word_b);

private:
    template<typename T>
    void add_record(const std::string& port_name, T& member, PortKind kind, Units units, bool optional_flag);

    template<typename T>
    static std::string pretty_type_name();

    void require_declaration_open(const std::string& port_name) const;
    void require_not_resolved(const char* method_name) const;

    void record_error(const std::string& message);
    void record_warning(const std::string& message);

    bool app_is_declared(const std::string& app_name) const;
    std::size_t app_index_of(const std::string& app_name) const;  // declared_app_count when not found
    PortRecord* find_port(const std::string& app_name, const std::string& port_name);

    std::string joined_input_names(const std::string& app_name) const;
    std::string joined_output_names(const std::string& app_name) const;
    std::string joined_publishers_of_type(const std::type_index& type) const;

    std::string closest_app_match(const std::string& query) const;
    std::string closest_input_match(const std::string& app_name, const std::string& query) const;
    std::string closest_output_match(const std::string& app_name, const std::string& query) const;

    void resolve_connection(const ConnectionRequest& request);
    void check_unconnected_inputs();
    void build_output_stamp_lists();

    static void consider_candidate(const std::string& candidate, const std::string& query,
                                   std::string& best_match, std::size_t& best_distance);
    static std::size_t suggestion_threshold(const std::string& query);

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
    // total_error_count can exceed stored_error_count if the array fills;
    // the thrown report says how many errors are shown
    std::array<std::string, SimConfig::max_wiring_report_number> wiring_errors;
    std::size_t stored_error_count = 0;
    std::size_t total_error_count  = 0;

    std::array<std::string, SimConfig::max_wiring_report_number> wiring_warnings;
    std::size_t wiring_warning_count = 0;

    std::string app_open_for_declarations;  // empty when no declare_io() is in progress
    bool wiring_resolved = false;
};

#include "io_registry.tpp"

#endif
