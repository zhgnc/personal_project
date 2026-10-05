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

    throw_if_port_storage_full(port_name);

    if (check_port_name_is_valid(port_name) == false) {
        return;
    }

    if (check_port_name_is_unique(port_name) == false) {
        return;
    }

    PortRecord& record = port_records[port_record_count];

    record.owner_app  = declaring_app_name;
    record.port_name  = port_name;
    record.data_ptr   = &class_data_ptr;
    record.size_bytes = sizeof(T);
    record.type       = std::type_index(typeid(T));
    record.type_name  = PortTypeName<T>::get();
    record.kind       = kind;

    record.owner_app_numerical_index = declaring_app_numerical_index;

    record.is_optional            = optional_flag;
    record.source_numerical_index = std::numeric_limits<std::size_t>::max();
    record.connection_attempted   = false;
    record.last_write_usec        = std::numeric_limits<uint64_t>::max();

    port_record_count++;
}

#endif
