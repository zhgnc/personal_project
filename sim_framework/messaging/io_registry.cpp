// Declaration bookkeeping, lookup, and the two runtime operations.
// Connecting and validating the topology lives in io_registry_resolution.cpp.

#include "io_registry.hpp"

#include <cstring>

// ---------------- Declaration lifecycle ----------------

// Opens the declaration phase for one app. Every port declared until
// end_declarations() belongs to this app, which is how ports get an owner
// without every declaration repeating the app name
void IoRegistry::begin_declarations(const std::string& app_name) {
    require_not_resolved("begin_declarations");

    if (app_open_for_declarations.empty() == false) {
        throw std::logic_error("IoRegistry::begin_declarations('" + app_name + "') called while declarations for '" +
                               app_open_for_declarations + "' are still open");
    }

    std::string violation = PortMatching::name_violation(app_name);

    if (violation.empty() == false) {
        throw std::logic_error("IoRegistry: app instance name '" + app_name + "' is invalid: " + violation);
    }

    if (app_is_declared(app_name)) {
        throw std::logic_error("IoRegistry: two apps share the instance name '" + app_name +
                               "': names must be unique because ports are addressed as 'app_name.port_name'");
    }

    if (declared_app_count == SimConfig::max_app_number) {
        throw std::logic_error(SimConfig::capacity_message("IoRegistry: declaring app '" + app_name + "'",
                                                           "max_app_number", SimConfig::max_app_number));
    }

    declared_app_names[declared_app_count] = app_name;
    declared_app_count++;
    app_open_for_declarations = app_name;
}

void IoRegistry::end_declarations() {
    if (app_open_for_declarations.empty()) {
        throw std::logic_error("IoRegistry::end_declarations() called with no declaration phase open");
    }

    app_open_for_declarations.clear();
}

// Declaring a port outside declare_io() would bind a pointer the framework
// never validated, so the phase is enforced rather than documented
void IoRegistry::require_declaration_open(const std::string& port_name) const {
    if (app_open_for_declarations.empty()) {
        throw std::logic_error("IoRegistry: declaration of port '" + port_name +
                               "' outside declare_io(): sub/tlm/tlm_debug are only legal while the "
                               "framework is collecting an app's IO, not from constructors, configure_model(), "
                               "or step()");
    }
}

void IoRegistry::require_not_resolved(const char* method_name) const {
    if (wiring_resolved) {
        throw std::logic_error(std::string("IoRegistry::") + method_name +
                               "() called after wiring was resolved: the IO topology is frozen once "
                               "resolve_and_validate() succeeds");
    }
}

// ---------------- Lookup ----------------

bool IoRegistry::app_is_declared(const std::string& app_name) const {
    return app_index_of(app_name) != declared_app_count;
}

std::size_t IoRegistry::app_index_of(const std::string& app_name) const {
    for (std::size_t i = 0; i < declared_app_count; i++) {
        if (declared_app_names[i] == app_name) {
            return i;
        }
    }

    return declared_app_count;
}

std::size_t IoRegistry::find_port_index(const std::string& app_name, const std::string& port_name) const {
    for (std::size_t i = 0; i < port_record_count; i++) {
        if (port_records[i].owner_app == app_name && port_records[i].port_name == port_name) {
            return i;
        }
    }

    return port_record_count;
}

// ---------------- Runtime ----------------

const AppIoInfo& IoRegistry::plan_for(const std::string& app_name) const {
    if (wiring_resolved == false) {
        throw std::logic_error("IoRegistry::plan_for('" + app_name + "') called before resolve_and_validate()");
    }

    std::size_t app_index = app_index_of(app_name);

    if (app_index == declared_app_count) {
        throw std::logic_error("IoRegistry::plan_for(): unknown app '" + app_name + "'");
    }

    return app_plans[app_index];
}

// The whole runtime cost of data passing: a flat list of memcpys resolved
// once at startup, with no lookups, strings, or allocation per step.
//
// The memcpy is safe because both ends were checked before the binding was
// built: add_record() static_asserts that the port's type is trivially
// copyable, and check_payload_match() rejects the connection unless source
// and destination are the exact same type
void IoRegistry::copy_inputs(const AppIoInfo& plan) const {
    for (std::size_t i = 0; i < plan.input_copy_count; i++) {
        const CopyBinding& binding = plan.input_copies[i];

        std::memcpy(binding.destination_ptr, binding.source_ptr, binding.size_bytes);
    }
}

void IoRegistry::stamp_outputs(const AppIoInfo& plan, uint64_t sim_time_usec) {
    for (std::size_t i = 0; i < plan.output_record_count; i++) {
        port_records[plan.output_record_indices[i]].last_write_usec = sim_time_usec;
    }
}
