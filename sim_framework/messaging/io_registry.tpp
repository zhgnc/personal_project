#ifndef IO_REGISTRY_TPP
#define IO_REGISTRY_TPP

#include "io_registry.hpp"

template<typename T>
void IoRegistry::sub(const std::string& port_name, T& destination) {
    add_record(port_name, destination, PortKind::Subscribe, false);
}

template<typename T>
void IoRegistry::sub_optional(const std::string& port_name, T& destination, const T& default_value) {
    destination = default_value; // Protects against users not using default values

    add_record(port_name, destination, PortKind::Subscribe, true);
}

template<typename T>
void IoRegistry::tlm(const std::string& port_name, T& source) {
    add_record(port_name, source, PortKind::TlmReq, false);
}

template<typename T>
void IoRegistry::tlm_debug(const std::string& port_name, T& source) {
    add_record(port_name, source, PortKind::TlmDebug, false);
}

template<typename T>
void IoRegistry::add_record(const std::string& port_name, T& class_data_ptr, PortKind kind, bool optional_flag) {
    // Trivial copyability makes "memcpy is a valid transport" a compile-time
    // guarantee. Anything owning heap memory (std::string, std::vector, ...) 
    // is rejected here instead of corrupting data at runtime
    static_assert(std::is_trivially_copyable_v<T>, "Port data must be trivially copyable. Use plain structs, math types, or "
                                                    "scalars; types like std::string or std::vector cannot cross a port.");

    require_declaration_open(port_name);

    if (port_record_count == SimConfig::max_port_number) {
        throw std::logic_error(SimConfig::capacity_message(
            "IoRegistry: declaring port '" + app_open_for_declarations + "." + port_name + "'",
            "max_port_number", SimConfig::max_port_number));
    }

    std::string violation = PortMatching::name_violation(port_name);

    if (violation.empty() == false) {
        wiring_report.record_error(app_open_for_declarations + ": port name '" + port_name + "' is invalid: " + violation);
        return;
    }

    bool duplicate_port_name = find_port_index(app_open_for_declarations, port_name) != port_record_count; // Inputs and outputs must all be unique
    
    if (duplicate_port_name == true) {
        wiring_report.record_error(app_open_for_declarations + ": port '" + port_name + "' is declared more than once");
        return;
    }

    PortRecord& record = port_records[port_record_count];

    record.owner_app  = app_open_for_declarations;
    record.port_name  = port_name;
    record.data_ptr   = &class_data_ptr;
    record.size_bytes = sizeof(T);
    record.type       = std::type_index(typeid(T));
    record.type_name  = PortTypeName<T>::get();
    record.kind       = kind;

    record.is_optional            = optional_flag;
    record.source_numerical_index = std::numeric_limits<std::size_t>::max();
    record.connection_attempted   = false;
    record.last_write_usec        = std::numeric_limits<uint64_t>::max();

    port_record_count++;
}


#endif
