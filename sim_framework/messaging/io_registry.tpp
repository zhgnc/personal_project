#ifndef IO_REGISTRY_TPP
#define IO_REGISTRY_TPP

#include "io_registry.hpp"

#if defined(__GNUC__)
#include <cxxabi.h>
#endif

template<typename T>
void IoRegistry::subscribe(const std::string& port_name, T& destination, Units units) {
    add_record(port_name, destination, PortKind::Subscribe, units, false);
}

template<typename T>
void IoRegistry::subscribe_optional(const std::string& port_name, T& destination, const T& default_value, Units units) {
    destination = default_value;
    add_record(port_name, destination, PortKind::Subscribe, units, true);
}

template<typename T>
void IoRegistry::tlm_req(const std::string& port_name, T& source, Units units) {
    add_record(port_name, source, PortKind::TlmReq, units, false);
}

template<typename T>
void IoRegistry::debug_tlm(const std::string& port_name, T& source, Units units) {
    add_record(port_name, source, PortKind::DebugTlm, units, false);
}

template<typename T>
void IoRegistry::add_record(const std::string& port_name, T& member, PortKind kind, Units units, bool optional_flag) {
    // Trivial copyability makes "memcpy is a valid transport" a compile-time
    // guarantee: plain structs, math types, and scalars pass; anything owning
    // heap memory (std::string, std::vector, ...) is rejected here instead of
    // corrupting data at runtime
    static_assert(std::is_trivially_copyable_v<T>,
                  "Port data must be trivially copyable. Use plain structs, math types, or "
                  "scalars; types like std::string or std::vector cannot cross a port.");

    require_declaration_open(port_name);

    if (port_record_count == SimConfig::max_port_number) {
        throw std::logic_error("IoRegistry: declaring '" + app_open_for_declarations + "." + port_name +
                               "' exceeds SimConfig::max_port_number (" +
                               std::to_string(SimConfig::max_port_number) +
                               "): increase the limit in sim_config.hpp");
    }

    std::string violation = io_name_violation(port_name);

    if (violation.empty() == false) {
        record_error(app_open_for_declarations + ": port name '" + port_name + "' is invalid: " + violation);
        return;
    }

    if (find_port(app_open_for_declarations, port_name) != nullptr) {
        record_error(app_open_for_declarations + ": port '" + port_name +
                     "' is declared more than once (inputs and outputs share one namespace per app)");
        return;
    }

    PortRecord& record = port_records[port_record_count];

    record.owner_app  = app_open_for_declarations;
    record.port_name  = port_name;
    record.data_ptr   = &member;
    record.size_bytes = sizeof(T);
    record.type       = std::type_index(typeid(T));
    record.type_name  = pretty_type_name<T>();
    record.kind       = kind;
    record.units      = units;

    record.is_optional          = optional_flag;
    record.is_connected         = false;
    record.connection_attempted = false;
    record.connected_source.clear();
    record.last_write_usec      = PortRecord::never_written;

    port_record_count++;
}

template<typename T>
std::string IoRegistry::pretty_type_name() {
#if defined(__GNUC__)
    int status = 0;
    char* demangled = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status);
    std::string name = (status == 0 && demangled != nullptr) ? demangled : typeid(T).name();
    std::free(demangled);
    return name;
#else
    // MSVC's typeid names are already human-readable
    return typeid(T).name();
#endif
}

#endif
