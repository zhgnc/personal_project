#include "io_registry.hpp"

// Records a request without checking it. All checking happens in resolve_and_validate()
void IoRegistry::connect(const std::string& destination_app,
                         const std::string& input,
                         const std::string& from_app,
                         const std::string& from_port,
                         const std::string& config_file_name) {
    ConnectionRequest request;

    request.destination_app  = destination_app;
    request.destination_port = input;
    request.source_app       = from_app;
    request.source_port      = from_port;
    request.config_file_name = config_file_name;

    throw_if_connection_storage_full(request);

    connection_requests[connection_request_count] = request;
    connection_request_count++;
}

// Resolves every request and all problems found are printed
void IoRegistry::resolve_and_validate() {
    for (std::size_t i = 0; i < connection_request_count; i++) {
        resolve_connection(connection_requests[i]);
    }

    check_unconnected_inputs();
    build_output_stamp_lists();

    wiring_report.throw_if_errors("[IO WIRING] found before t=0:");
}

void IoRegistry::record_connection_error(const ConnectionRequest& request, const std::string& message) {
    wiring_report.record_error(message + "  (config file: " + request.config_file_name + ")");
}

void IoRegistry::resolve_connection(const ConnectionRequest& request) {
    std::size_t destination_index = find_port_index(request.destination_app, request.destination_port);
    std::size_t source_index      = find_port_index(request.source_app, request.source_port);

    bool destination_port_exists = destination_index != port_record_count;

    // Set before the checks so a connection that fails below gets only its own
    // error, not a second "has no connection" from check_unconnected_inputs()
    if (destination_port_exists == true) {
        port_records[destination_index].connection_attempted = true;
    }

    if (check_destination_port(request, destination_index) == false) {
        return;
    }

    if (check_source_port(request, source_index) == false) {
        return;
    }

    if (check_apps_are_different(request) == false) {
        return;
    }

    if (check_types_match(request, source_index, destination_index) == false) {
        return;
    }

    add_copy_binding(request, source_index, destination_index);
}

bool IoRegistry::check_destination_port(const ConnectionRequest& request, std::size_t destination_index) {
    bool destination_app_does_not_exist = is_app_declared(request.destination_app) == false;

    if (destination_app_does_not_exist == true) {
        record_connection_error(request, "connection destination app '" + request.destination_app +
                                         "' does not exist.\n       apps in this sim: " +
                                         PortMatching::list_app_names(declared_app_names, declared_app_count));
        return false;
    }

    bool destination_port_was_not_declared = destination_index == port_record_count;

    if (destination_port_was_not_declared == true) {
        record_connection_error(request, request.destination_app + ": input '" + request.destination_port +
                                         "' is not a declared subscription.\n       declared inputs: " +
                                         PortMatching::list_port_names(port_records, port_record_count,
                                                                       request.destination_app,
                                                                       PortDirection::Input));
        return false;
    }

    const PortRecord& destination = port_records[destination_index];

    bool destination_is_an_output = destination.is_input() == false;

    if (destination_is_an_output == true) {
        record_connection_error(request, request.destination_app + ": '" + request.destination_port + "' is a " +
                                         port_kind_to_string(destination.kind) +
                                         " output; only inputs can be connection destinations");
        return false;
    }

    bool destination_is_already_connected = destination.is_connected();

    if (destination_is_already_connected == true) {
        record_connection_error(request, request.destination_app + ": input '" + request.destination_port +
                                         "' is connected more than once (already connected to " +
                                         source_name_of(destination) + ")");
        return false;
    }

    return true;
}

bool IoRegistry::check_source_port(const ConnectionRequest& request, std::size_t source_index) {
    bool source_app_does_not_exist = is_app_declared(request.source_app) == false;

    if (source_app_does_not_exist == true) {
        record_connection_error(request, request.destination_app + "." + request.destination_port + ": source app '" +
                                         request.source_app + "' does not exist.\n       apps in this sim: " +
                                         PortMatching::list_app_names(declared_app_names, declared_app_count));
        return false;
    }

    bool source_port_was_not_declared = source_index == port_record_count;

    if (source_port_was_not_declared == true) {
        record_connection_error(request, request.destination_app + "." + request.destination_port + ": source '" +
                                         request.source_app + "." + request.source_port +
                                         "' is not a declared output.\n       outputs of " + request.source_app +
                                         ": " + PortMatching::list_port_names(port_records, port_record_count,
                                                                              request.source_app,
                                                                              PortDirection::Output));
        return false;
    }

    bool source_is_an_input = port_records[source_index].is_input();

    if (source_is_an_input == true) {
        record_connection_error(request, request.destination_app + "." + request.destination_port + ": source '" +
                                         request.source_app + "." + request.source_port +
                                         "' is an input; data must come from a tlm or tlm_debug output");
        return false;
    }

    return true;
}

// The data is already a member of that app, so a port would only copy it onto
// itself a frame later
bool IoRegistry::check_apps_are_different(const ConnectionRequest& request) {
    bool app_subscribes_to_its_own_output = request.source_app == request.destination_app;

    if (app_subscribes_to_its_own_output == true) {
        record_connection_error(request, request.destination_app + ": input '" + request.destination_port +
                                         "' cannot connect to '" + request.source_port +
                                         "' because an app cannot subscribe to its own output; that data is "
                                         "already a member of this app, so read it directly");
        return false;
    }

    return true;
}

bool IoRegistry::check_types_match(const ConnectionRequest& request, std::size_t source_index,
                                   std::size_t destination_index) {
    const PortRecord& source      = port_records[source_index];
    const PortRecord& destination = port_records[destination_index];

    bool types_do_not_match = source.type != destination.type;

    if (types_do_not_match == true) {
        record_connection_error(request, request.destination_app + "." + request.destination_port + " (" +
                                         destination.type_name + ") cannot connect to " + request.source_app + "." +
                                         request.source_port + " (" + source.type_name + "): type mismatch");
        return false;
    }

    return true;
}

// Marks the input connected and appends the copy its app will execute each time it steps
void IoRegistry::add_copy_binding(const ConnectionRequest& request, std::size_t source_index, std::size_t destination_index) {
    AppIoInfo& plan = app_plans[port_records[destination_index].owner_app_numerical_index];

    throw_if_app_input_storage_full(plan, request);

    const PortRecord& source = port_records[source_index];
    PortRecord& destination  = port_records[destination_index];

    destination.source_numerical_index = source_index;

    plan.input_copies[plan.input_copy_count] = CopyBinding{source.data_ptr, destination.data_ptr, destination.size_bytes};
    plan.input_copy_count++;
}

void IoRegistry::check_unconnected_inputs() {
    for (std::size_t i = 0; i < port_record_count; i++) {
        const PortRecord& record = port_records[i];

        bool needs_connection = record.is_input() == true && record.is_optional == false &&
                                record.is_connected() == false && record.connection_attempted == false;

        if (needs_connection == false) {
            continue;
        }

        wiring_report.record_error(record.owner_app + ": required input '" + record.port_name + "' (" +
                                   record.type_name + ") has no connection.");
    }
}

// Gives each app the indices of its own output records, so stamping after a
// step is a short indexed loop instead of a scan of every port in the sim
void IoRegistry::build_output_stamp_lists() {
    for (std::size_t i = 0; i < port_record_count; i++) {
        bool port_is_an_input = port_records[i].is_input();

        if (port_is_an_input == true) {
            continue;
        }

        AppIoInfo& plan = app_plans[port_records[i].owner_app_numerical_index];

        throw_if_app_output_storage_full(plan, port_records[i].owner_app);

        plan.output_record_indices[plan.output_record_count] = i;
        plan.output_record_count++;
    }
}

void IoRegistry::throw_if_connection_storage_full(const ConnectionRequest& request) const {
    bool connection_storage_is_full = connection_request_count >= SimConfig::max_connection_number;

    if (connection_storage_is_full == true) {
        throw std::logic_error(SimConfig::capacity_message(
            "IoRegistry: connection " + request.destination_app + "." + request.destination_port + " <- " +
            request.source_app + "." + request.source_port,
            "max_connection_number", SimConfig::max_connection_number));
    }
}

void IoRegistry::throw_if_app_input_storage_full(const AppIoInfo& plan, const ConnectionRequest& request) const {
    bool app_input_storage_is_full = plan.input_copy_count == SimConfig::max_app_input_number;

    if (app_input_storage_is_full == true) {
        throw std::logic_error(SimConfig::capacity_message(
            "IoRegistry: connecting " + request.destination_app + "." + request.destination_port,
            "max_app_input_number", SimConfig::max_app_input_number));
    }
}

void IoRegistry::throw_if_app_output_storage_full(const AppIoInfo& plan, const std::string& owner_app) const {
    bool app_output_storage_is_full = plan.output_record_count == SimConfig::max_app_output_number;

    if (app_output_storage_is_full == true) {
        throw std::logic_error(SimConfig::capacity_message(
            "IoRegistry: outputs of " + owner_app,
            "max_app_output_number", SimConfig::max_app_output_number));
    }
}
