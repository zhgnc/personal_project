#ifndef PORT_DATA_HPP
#define PORT_DATA_HPP

// The data structures of the sim framework's messaging architecture.
//
// A port is one piece of app data the framework knows about: a name, a type,
// and the address of a member inside an app's own model struct. Everything
// here is plain data - the behavior that fills these structures in lives in
// IoRegistry (io_registry.hpp).

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <typeindex>
#include <typeinfo>

#include "../sim_config.hpp"


enum class PortKind : uint8_t {
    Subscribe = 0,
    TlmReq    = 1,
    TlmDebug  = 2,
};

enum class PortDirection : uint8_t {
    Input,   
    Output,
};

enum class AppTlmLevel : uint8_t {
    Off      = 0,
    Required = 1,
    Debug    = 2,
};

inline std::string tlm_level_to_string(AppTlmLevel level) {
    switch (level) {
        case AppTlmLevel::Off:      return "off";
        case AppTlmLevel::Required: return "required";
        case AppTlmLevel::Debug:    return "debug";
        default:                 return "Error Reporting AppTlmLevel Enum!";
    }
}

inline bool tlm_level_from_string(const std::string& text, AppTlmLevel& level_out) {
    if (text == "off")      { level_out = AppTlmLevel::Off;      return true; }
    if (text == "required") { level_out = AppTlmLevel::Required; return true; }
    if (text == "debug")    { level_out = AppTlmLevel::Debug;    return true; }
    
    return false;
}

inline std::string port_kind_to_string(PortKind kind) {
    switch (kind) {
        case PortKind::Subscribe: return "sub";
        case PortKind::TlmReq:    return "tlm";
        case PortKind::TlmDebug:  return "tlm_debug";
        default:                  return "Error Reporting PortKind Enum!";
    }
}

// Struct contains everything the framework knows about an input/output
struct PortRecord {
    std::string owner_app;
    std::string port_name;
    PortKind kind = PortKind::Subscribe;

    void* data_ptr         = nullptr;
    std::size_t size_bytes = 0;                             // Required to copy data safely
    std::type_index type   = std::type_index(typeid(void)); // Required to catch mismatched connection at startup 
    std::string type_name;                                  // Required for human readable error messages

    // Subscribe ports
    bool is_optional                   = false; 
    std::size_t source_numerical_index = std::numeric_limits<std::size_t>::max(); // Index of the output port feeding this input. Max value when not connected
    bool connection_attempted          = false;

    // Output ports
    uint64_t last_write_usec = std::numeric_limits<uint64_t>::max();

    bool is_connected() const { return source_numerical_index != std::numeric_limits<std::size_t>::max(); }
};

struct CopyBinding {
    const void* source_ptr      = nullptr;
    void*       destination_ptr = nullptr;
    std::size_t size_bytes      = 0;
};

struct AppIoInfo {
    std::array<CopyBinding, SimConfig::max_app_input_number> input_copies;
    std::size_t input_copy_count = 0;

    std::array<std::size_t, SimConfig::max_app_output_number> output_record_indices;
    std::size_t output_record_count = 0;
};

struct IoConfigSpec {
    std::string destination_app;
    std::string input;
    std::string from_app;
    std::string from_port;
    std::string config_file_name;
};

struct AppTlmSettings {
    std::string app_name;
    double      rate_hz = 0.0;
    AppTlmLevel level   = AppTlmLevel::Off;
};

struct ConnectionRequest {
    std::string destination_app;
    std::string destination_port;
    std::string source_app;
    std::string source_port;
    std::string config_file_name; 
};

#endif
