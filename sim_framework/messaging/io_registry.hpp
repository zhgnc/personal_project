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

enum class PortKind : uint8_t {
    Subscribe = 0,
    TlmReq    = 1,
    TlmDebug  = 2,
};

enum class PortDirection : uint8_t {
    Input,   // Subscribe ports
    Output,  // TlmReq and TlmDebug ports
};

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

inline bool tlm_level_from_string(const std::string& text, TlmLevel& level_out) {
    if (text == "off")      { level_out = TlmLevel::Off;      return true; }
    if (text == "required") { level_out = TlmLevel::Required; return true; }
    if (text == "debug")    { level_out = TlmLevel::Debug;    return true; }

    // Returns false when `text` is not one of the level names, so the caller can
    // report the bad value with its own file and line context
    return false;
}

inline std::string port_kind_to_string(PortKind kind) {
    switch (kind) {
        case PortKind::Subscribe: return "sub";
        case PortKind::TlmReq:    return "tlm_req";
        case PortKind::TlmDebug:  return "tlm_debug";
        default:                  return "Error Reporting PortKind Enum!";
    }
}

// Struct contains everything the framework knows about one input/output. 
// Every member has a default so unused slots in the registry's fixed-size 
// storage are well defined.
struct PortRecord {
    // Set to max value meaning this input has no source yet
    static constexpr std::size_t no_source = std::numeric_limits<std::size_t>::max();

    // Set to max value meaning "the owning app has not stepped yet"
    static constexpr uint64_t never_written = std::numeric_limits<uint64_t>::max();

    std::string owner_app;
    std::string port_name;

    // Address of the app member this port is bound to, plus the type
    // information needed to check connections and copy the bytes safely.
    // `type` is what makes a mismatched connection a startup error instead of
    // garbage data; `type_name` is the readable form used in messages
    void*           data_ptr   = nullptr;
    std::size_t     size_bytes = 0;
    std::type_index type       = std::type_index(typeid(void));
    std::string     type_name;

    PortKind kind  = PortKind::Subscribe;

    // ---- Subscribe ports only ----
    bool is_optional = false;  // declared with sub_optional(): unconnected is legal, the default value stands

    // Index into the registry's port records of the output feeding this
    // input. This is the single record of whether an input is connected
    std::size_t source_record_index = no_source;

    // A connect() targeted this input, even if it then failed validation.
    // Distinct from being connected: it suppresses the generic
    // "required input has no connection" sweep for an input that already
    // produced a specific error
    bool connection_attempted = false;

    // ---- Output ports only ----
    // Sim time the owning app last stepped. Lets a subscriber ask how old its
    // data is, which is the only way to detect the "publisher stopped
    // stepping, subscriber is holding a stale value" case
    uint64_t last_write_usec = never_written;

    bool is_connected() const { return source_record_index != no_source; }
};

// One pre-resolved memcpy from a publisher's member into a subscriber's
// member. Built once at wiring time and executed every step.
//
// Only resolve_connection() creates these, and only after it has confirmed
// both ports are the same type, so the raw pointers here are always a
// matched pair
struct CopyBinding {
    const void* source_ptr      = nullptr;
    void*       destination_ptr = nullptr;
    std::size_t size_bytes      = 0;
};

// Per-app runtime view handed to the frame loop once after wiring resolves:
// the input copies to execute before the app steps, and the indices of the
// app's output records to time-stamp after it steps
struct AppIoPlan {
    std::array<CopyBinding, SimConfig::max_app_input_number> input_copies;
    std::size_t input_copy_count = 0;

    std::array<std::size_t, SimConfig::max_app_output_number> output_record_indices;
    std::size_t output_record_count = 0;
};

// One input connection with every field explicit, as parsed from an app's
// `input_mapping:` block and replayed into every run's own registry
struct IoConnectionSpec {
    std::string destination_app;
    std::string input;
    std::string from_app;
    std::string from_port;
    std::string origin;   // "<config file>:<line>", quoted in error messages
};

// Per-app telemetry settings from the app manifest, applied by the recorder
// when it registers HDF5 datasets
struct AppTlmSettings {
    std::string app_name;
    double      rate_hz = 0.0;
    TlmLevel    level   = TlmLevel::Off;
};

// A connection request recorded by connect() and checked during
// resolve_and_validate()
struct ConnectionRequest {
    std::string destination_app;
    std::string destination_port;
    std::string source_app;
    std::string source_port;
    std::string origin;
};

// Thrown with a report listing every wiring or config problem found, not just
// the first one. Also used by the yaml parsers so all startup configuration
// failures surface as one exception type
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
//     stepping has no unconnected required inputs, no type mismatches, and
//     no unit mismatches.
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

    // ---------------- Shared name utilities ----------------

    // App instance and port names may use only lowercase letters, digits, and
    // underscores. Returns an empty string when `name` complies, otherwise a
    // description of the violation for an error message
    static std::string io_name_violation(const std::string& name);

    // Levenshtein distance, shared by every "did you mean" suggestion
    static std::size_t edit_distance(const std::string& word_a, const std::string& word_b);

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

    static bool port_matches(const PortRecord& record, PortDirection direction);

    // ---- Error-message builders ----
    std::string joined_port_names(const std::string& app_name, PortDirection direction) const;
    std::string joined_publishers_of_type(const std::type_index& type) const;

    std::string closest_app_match(const std::string& query) const;
    std::string closest_port_match(const std::string& app_name, PortDirection direction, const std::string& query) const;

    // Appends " did you mean 'x'?" when a near match was found, otherwise ""
    static std::string suggestion_text(const std::string& suggestion);

    // Keeps `best_match`/`best_distance` updated with the closest candidate
    static void consider_candidate(const std::string& candidate, const std::string& query,
                                   std::string& best_match, std::size_t& best_distance);

    // Largest edit distance still treated as a plausible typo for `query`
    static std::size_t suggestion_threshold(const std::string& query);

    // ---- resolve_and_validate() stages ----
    void resolve_connection(const ConnectionRequest& request);
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
