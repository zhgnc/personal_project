#ifndef PORT_DATA_HPP
#define PORT_DATA_HPP

// The data structures of the sim framework's data passing.
//
// A port is one piece of app data the framework knows about: a name, a type,
// and the address of a member inside an app's own model struct. Everything
// here is plain data - the behavior that fills these structures in lives in
// IoRegistry (io_registry.hpp).
//
// This header is kept separate from the registry so the config parsers can
// use the shared types (IoConnectionSpec, IoWiringError) without pulling in
// the whole port system.

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <typeinfo>

#include "../sim_config.hpp"

// Subscribe ports are inputs filled by the framework before each step. TlmReq
// and TlmDebug ports are outputs: both are available to every other app as
// connection sources, the kind only controls whether the recorder stores them
// at the app's configured telemetry level.
enum class PortKind : uint8_t {
    Subscribe = 0,
    TlmReq    = 1,
    TlmDebug  = 2,
};

// Which side of an app a lookup is about. Used by the helpers that list port
// names, so call sites read as words instead of booleans
enum class PortDirection : uint8_t {
    Input,   // Subscribe ports
    Output,  // TlmReq and TlmDebug ports
};

// Per-app recording level: Off records nothing, Required records tlm_req
// ports only, Debug records tlm_req and tlm_debug ports. Data passing is
// unaffected - unrecorded ports are still available to other apps
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

#endif
