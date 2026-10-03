// Connecting apps together and validating the result.
//
// Everything here runs once per simulation run, between the apps declaring
// their ports and the first step. Nothing in this file executes during the
// frame loop.

#include "io_registry.hpp"

// Records a request without checking it: apps declare their ports in priority
// order, so a connection is routinely recorded before its source app has
// declared anything. All checking happens in resolve_and_validate()
void IoRegistry::connect(const std::string& destination_app,
                         const std::string& input,
                         const std::string& from_app,
                         const std::string& from_port,
                         const std::string& config_file_name) {
    require_not_resolved("connect");

    if (connection_request_count == SimConfig::max_connection_number) {
        throw std::logic_error(SimConfig::capacity_message(
            "IoRegistry: connection " + destination_app + "." + input + " <- " + from_app + "." + from_port,
            "max_connection_number", SimConfig::max_connection_number));
    }

    ConnectionRequest request;

    request.destination_app  = destination_app;
    request.destination_port = input;
    request.source_app       = from_app;
    request.source_port      = from_port;
    request.config_file_name = config_file_name;

    connection_requests[connection_request_count] = request;
    connection_request_count++;
}

// Resolves every request, then sweeps for inputs nothing connected, then
// builds the per-app plans. Every problem found is collected so the user sees
// the complete list from one run instead of fixing them one at a time
void IoRegistry::resolve_and_validate() {
    require_not_resolved("resolve_and_validate");

    if (app_open_for_declarations.empty() == false) {
        throw std::logic_error("IoRegistry::resolve_and_validate() called while declarations for " +
                               app_open_for_declarations + " are still open");
    }

    for (std::size_t i = 0; i < connection_request_count; i++) {
        resolve_connection(connection_requests[i]);
    }

    check_unconnected_inputs();
    build_output_stamp_lists();

    wiring_report.throw_if_errors("[IO WIRING] found before t=0:");

    wiring_resolved = true;
}

// Checks one requested connection and, if it is sound, records the memcpy
// that will carry the data. The checks run destination side, then source
// side, then payload; the first failure returns, because later checks would
// only produce more noise from the same cause
void IoRegistry::resolve_connection(const ConnectionRequest& request) {
    const std::string context = "  (config file: " + request.config_file_name + ")";

    std::size_t destination_index = 0;
    std::size_t source_index      = 0;

    if (check_destination_port(request, context, destination_index) == false) {
        return;
    }

    if (check_source_port(request, context, source_index) == false) {
        return;
    }

    if (check_payload_match(request, context, port_records[source_index], port_records[destination_index]) == false) {
        return;
    }

    add_copy_binding(request, source_index, destination_index);
}

// The destination must be an input, on an app that exists, that nothing has
// already connected
bool IoRegistry::check_destination_port(const ConnectionRequest& request, const std::string& context,
                                        std::size_t& destination_index_out) {
    if (app_is_declared(request.destination_app) == false) {
        wiring_report.record_error("connection destination app '" + request.destination_app + "' does not exist."
                                   "\n       apps in this sim: " +
                                   PortMatching::list_app_names(declared_app_names, declared_app_count) + context);
        return false;
    }

    std::size_t destination_index = find_port_index(request.destination_app, request.destination_port);

    if (destination_index == port_record_count) {
        wiring_report.record_error(request.destination_app + ": input '" + request.destination_port +
                                   "' is not a declared subscription.\n       declared inputs: " +
                                   PortMatching::list_port_names(port_records, port_record_count,
                                                                 request.destination_app, PortDirection::Input) +
                                   context);
        return false;
    }

    PortRecord& destination = port_records[destination_index];

    if (destination.kind != PortKind::Subscribe) {
        wiring_report.record_error(request.destination_app + ": '" + request.destination_port + "' is a " +
                                   port_kind_to_string(destination.kind) +
                                   " output; only inputs can be connection destinations" + context);
        return false;
    }

    // From here on this input has an attempt on record: if a later check
    // fails, the specific error is enough and the port is left out of the
    // generic unconnected-input sweep
    destination.connection_attempted = true;

    if (destination.is_connected()) {
        wiring_report.record_error(request.destination_app + ": input '" + request.destination_port +
                                   "' is connected more than once (already connected to " +
                                   source_name_of(destination) + ")" + context);
        return false;
    }

    destination_index_out = destination_index;
    return true;
}

// The source must be an output, on an app that exists
bool IoRegistry::check_source_port(const ConnectionRequest& request, const std::string& context,
                                   std::size_t& source_index_out) {
    if (app_is_declared(request.source_app) == false) {
        wiring_report.record_error(request.destination_app + "." + request.destination_port + ": source app '" +
                                   request.source_app + "' does not exist.\n       apps in this sim: " +
                                   PortMatching::list_app_names(declared_app_names, declared_app_count) + context);
        return false;
    }

    std::size_t source_index = find_port_index(request.source_app, request.source_port);

    if (source_index == port_record_count) {
        wiring_report.record_error(request.destination_app + "." + request.destination_port + ": source '" +
                                   request.source_app + "." + request.source_port +
                                   "' is not a declared output.\n       outputs of " + request.source_app + ": " +
                                   PortMatching::list_port_names(port_records, port_record_count,
                                                                 request.source_app, PortDirection::Output) +
                                   context);
        return false;
    }

    if (port_records[source_index].kind == PortKind::Subscribe) {
        wiring_report.record_error(request.destination_app + "." + request.destination_port + ": source '" +
                                   request.source_app + "." + request.source_port + "' is an input; data must come "
                                   "from a tlm or tlm_debug output" + context);
        return false;
    }

    source_index_out = source_index;
    return true;
}

// This is the check that makes the raw memcpy in copy_inputs() safe: no
// CopyBinding is created unless both ends are the exact same type, so a
// mismatched connection is a startup error rather than wrong bytes at runtime
bool IoRegistry::check_payload_match(const ConnectionRequest& request, const std::string& context,
                                     const PortRecord& source, const PortRecord& destination) {
    if (source.type != destination.type) {
        wiring_report.record_error(request.destination_app + "." + request.destination_port + " (" +
                                   destination.type_name + ") cannot connect to " + request.source_app + "." +
                                   request.source_port + " (" + source.type_name + "): type mismatch" + context);
        return false;
    }

    return true;
}

// Marks the input connected and appends the copy its app will execute each
// time it steps
void IoRegistry::add_copy_binding(const ConnectionRequest& request, std::size_t source_index,
                                  std::size_t destination_index) {
    // Legal (an app may feed its own previous frame output forward) but
    // usually a wiring mistake, so it is surfaced as a warning
    if (request.source_app == request.destination_app) {
        wiring_report.record_warning(request.destination_app + " subscribes to its own output '" +
                                     request.source_port + "'");
    }

    AppIoInfo& plan = app_plans[app_index_of(request.destination_app)];

    if (plan.input_copy_count == SimConfig::max_app_input_number) {
        throw std::logic_error(SimConfig::capacity_message(
            "IoRegistry: connecting " + request.destination_app + "." + request.destination_port,
            "max_app_input_number", SimConfig::max_app_input_number));
    }

    const PortRecord& source = port_records[source_index];
    PortRecord& destination  = port_records[destination_index];

    destination.source_numerical_index = source_index;

    plan.input_copies[plan.input_copy_count] = CopyBinding{source.data_ptr, destination.data_ptr,
                                                           destination.size_bytes};
    plan.input_copy_count++;
}

// Reports every required input that no config file connected. Inputs whose
// connection was attempted and failed are skipped: they already produced a
// specific error, and repeating them here would double every mistake
void IoRegistry::check_unconnected_inputs() {
    for (std::size_t i = 0; i < port_record_count; i++) {
        const PortRecord& record = port_records[i];

        bool needs_connection = record.kind == PortKind::Subscribe && record.is_optional == false &&
                                record.is_connected() == false && record.connection_attempted == false;

        if (needs_connection == false) {
            continue;
        }

        // Listing the ports that would fit turns "what do I connect this to?"
        // into a copy and paste
        std::string publishers = PortMatching::list_publishers_of_type(port_records, port_record_count, record.type);
        std::string message    = record.owner_app + ": required input '" + record.port_name + "' (" +
                                 record.type_name + ") has no connection.";

        if (publishers.empty()) {
            message += "\n       no app publishes this type";
        } else {
            message += "\n       available publishers of " + record.type_name + ": " + publishers;
        }

        wiring_report.record_error(message);
    }
}

// Gives each app the indices of its own output records, so stamping after a
// step is a short indexed loop instead of a scan of every port in the sim
void IoRegistry::build_output_stamp_lists() {
    for (std::size_t i = 0; i < port_record_count; i++) {
        if (port_records[i].kind == PortKind::Subscribe) {
            continue;
        }

        AppIoInfo& plan = app_plans[app_index_of(port_records[i].owner_app)];

        if (plan.output_record_count == SimConfig::max_app_output_number) {
            throw std::logic_error(SimConfig::capacity_message(
                "IoRegistry: outputs of " + port_records[i].owner_app,
                "max_app_output_number", SimConfig::max_app_output_number));
        }

        plan.output_record_indices[plan.output_record_count] = i;
        plan.output_record_count++;
    }
}
