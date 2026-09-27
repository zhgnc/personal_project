// Implementation of IoRegistry: declaration bookkeeping, connection
// resolution and validation, the runtime copy/stamp operations, and the
// startup report. See io_registry.hpp for the class contract.

#include "io_registry.hpp"

#include <algorithm>
#include <cstring>
#include <sstream>

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

    std::string violation = io_name_violation(app_name);

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
                               "' outside declare_io(): sub/tlm_req/tlm_debug are only legal while the "
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

// ---------------- Error and warning collection ----------------

// Stores the first max_wiring_report_number messages but counts them all, so
// a flood of errors still reports an accurate total
void IoRegistry::record_error(const std::string& message) {
    if (stored_error_count < wiring_errors.size()) {
        wiring_errors[stored_error_count] = message;
        stored_error_count++;
    }

    total_error_count++;
}

void IoRegistry::record_warning(const std::string& message) {
    if (wiring_warning_count < wiring_warnings.size()) {
        wiring_warnings[wiring_warning_count] = message;
        wiring_warning_count++;
    }

    total_warning_count++;
}

// ---------------- Connection recording and resolution ----------------

// Records a request without checking it: apps declare their ports in
// priority order, so a connection is routinely recorded before its source
// app has declared anything. All checking happens in resolve_and_validate()
void IoRegistry::connect(const std::string& destination_app, const std::string& input,
                         const std::string& from_app, const std::string& from_port,
                         const std::string& origin) {
    require_not_resolved("connect");

    if (connection_request_count == SimConfig::max_connection_number) {
        throw std::logic_error(SimConfig::capacity_message(
            "IoRegistry: connection '" + destination_app + "." + input + " <- " + from_app + "." + from_port + "'",
            "max_connection_number", SimConfig::max_connection_number));
    }

    ConnectionRequest request;

    request.destination_app  = destination_app;
    request.destination_port = input;
    request.source_app       = from_app;
    request.source_port      = from_port;
    request.origin           = origin;

    connection_requests[connection_request_count] = request;
    connection_request_count++;
}

// Resolves every request, then sweeps for inputs nothing connected, then
// builds the per-app plans. Every problem found is collected so the user
// sees the complete list from one run instead of fixing them one at a time
void IoRegistry::resolve_and_validate() {
    require_not_resolved("resolve_and_validate");

    if (app_open_for_declarations.empty() == false) {
        throw std::logic_error("IoRegistry::resolve_and_validate() called while declarations for '" +
                               app_open_for_declarations + "' are still open");
    }

    for (std::size_t i = 0; i < connection_request_count; i++) {
        resolve_connection(connection_requests[i]);
    }

    check_unconnected_inputs();
    build_output_stamp_lists();

    if (total_error_count > 0) {
        std::ostringstream report;
        report << "[IO WIRING] " << total_error_count << " error(s) found before t=0:";

        if (total_error_count > stored_error_count) {
            report << "  (showing first " << stored_error_count << ")";
        }
        report << "\n";

        for (std::size_t i = 0; i < stored_error_count; i++) {
            report << "\n  " << (i + 1) << ". " << wiring_errors[i] << "\n";
        }

        throw IoWiringError(report.str());
    }

    wiring_resolved = true;
}

// Checks one requested connection and, if it is sound, appends the memcpy
// that will carry the data. The checks run destination-side first, then
// source-side, then payload compatibility; the first failure reports and
// returns, because later checks would only produce noise from the same cause
void IoRegistry::resolve_connection(const ConnectionRequest& request) {
    const std::string context = "  (origin: " + request.origin + ")";

    // ---- destination must be a declared input on an existing app ----
    if (app_is_declared(request.destination_app) == false) {
        record_error("connection destination app '" + request.destination_app + "' does not exist." +
                     suggestion_text(closest_app_match(request.destination_app)) + context);
        return;
    }

    std::size_t destination_index = find_port_index(request.destination_app, request.destination_port);

    if (destination_index == port_record_count) {
        record_error(request.destination_app + ": input '" + request.destination_port +
                     "' is not a declared subscription.\n       declared inputs: " +
                     joined_port_names(request.destination_app, PortDirection::Input) +
                     suggestion_text(closest_port_match(request.destination_app, PortDirection::Input,
                                                        request.destination_port)) + context);
        return;
    }

    PortRecord& destination = port_records[destination_index];

    if (destination.kind != PortKind::Subscribe) {
        record_error(request.destination_app + ": '" + request.destination_port + "' is a " +
                     port_kind_to_string(destination.kind) +
                     " output; only inputs can be connection destinations" + context);
        return;
    }

    // From here on this input has an attempt on record: if it fails below,
    // the specific error is enough and the port is excluded from the generic
    // unconnected-input sweep
    destination.connection_attempted = true;

    if (destination.is_connected()) {
        record_error(request.destination_app + ": input '" + request.destination_port +
                     "' is connected more than once (already connected to " + source_name_of(destination) + ")" +
                     context);
        return;
    }

    // ---- source must be a declared output on an existing app ----
    if (app_is_declared(request.source_app) == false) {
        record_error(request.destination_app + "." + request.destination_port + ": source app '" +
                     request.source_app + "' does not exist." +
                     suggestion_text(closest_app_match(request.source_app)) + context);
        return;
    }

    std::size_t source_index = find_port_index(request.source_app, request.source_port);

    if (source_index == port_record_count) {
        record_error(request.destination_app + "." + request.destination_port + ": source '" + request.source_app +
                     "." + request.source_port + "' is not a declared output.\n       outputs of " +
                     request.source_app + ": " + joined_port_names(request.source_app, PortDirection::Output) +
                     suggestion_text(closest_port_match(request.source_app, PortDirection::Output,
                                                        request.source_port)) + context);
        return;
    }

    const PortRecord& source = port_records[source_index];

    if (source.kind == PortKind::Subscribe) {
        record_error(request.destination_app + "." + request.destination_port + ": source '" + request.source_app +
                     "." + request.source_port + "' is an input; " +
                     "data must come from a tlm_req or tlm_debug output" + context);
        return;
    }

    // ---- payloads must agree ----
    // This is the check that makes the raw memcpy in copy_inputs() safe: no
    // CopyBinding is ever created unless both ends are the exact same type,
    // so a mismatched connection is a startup error rather than a copy of
    // the wrong bytes at runtime
    if (source.type != destination.type) {
        record_error(request.destination_app + "." + request.destination_port + " (" + destination.type_name +
                     ") cannot connect to " + request.source_app + "." + request.source_port + " (" +
                     source.type_name + "): type mismatch" + context);
        return;
    }

    // Legal (an app may feed its own previous-frame output forward) but
    // usually a wiring mistake, so it is surfaced as a warning
    if (request.source_app == request.destination_app) {
        record_warning(request.destination_app + " subscribes to its own output '" + request.source_port + "'" +
                       context);
    }

    AppIoPlan& plan = app_plans[app_index_of(request.destination_app)];

    if (plan.input_copy_count == SimConfig::max_app_input_number) {
        throw std::logic_error(SimConfig::capacity_message(
            "IoRegistry: connecting '" + request.destination_app + "." + request.destination_port + "'",
            "max_app_input_number", SimConfig::max_app_input_number));
    }

    destination.source_record_index = source_index;

    plan.input_copies[plan.input_copy_count] = CopyBinding{source.data_ptr, destination.data_ptr, destination.size_bytes};
    plan.input_copy_count++;
}

// Reports every required input that no config file connected. Inputs whose
// connection was attempted and failed are skipped: they already produced a
// specific error, and repeating them here would double every typo
void IoRegistry::check_unconnected_inputs() {
    for (std::size_t i = 0; i < port_record_count; i++) {
        const PortRecord& record = port_records[i];

        bool needs_connection = record.kind == PortKind::Subscribe && record.is_optional == false &&
                                record.is_connected() == false && record.connection_attempted == false;

        if (needs_connection == false) {
            continue;
        }

        // Listing the ports that would fit turns "what do I connect this to?"
        // into a copy-paste
        std::string publishers = joined_publishers_of_type(record.type);
        std::string message = record.owner_app + ": required input '" + record.port_name + "' (" +
                              record.type_name + ") has no connection.";

        if (publishers.empty()) {
            message += "\n       no app publishes this type";
        } else {
            message += "\n       available publishers of " + record.type_name + ": " + publishers;
        }

        record_error(message);
    }
}

// Gives each app the indices of its own output records, so stamp_outputs()
// after a step is a short indexed loop instead of a scan of every port
void IoRegistry::build_output_stamp_lists() {
    for (std::size_t i = 0; i < port_record_count; i++) {
        if (port_records[i].kind == PortKind::Subscribe) {
            continue;
        }

        AppIoPlan& plan = app_plans[app_index_of(port_records[i].owner_app)];

        if (plan.output_record_count == SimConfig::max_app_output_number) {
            throw std::logic_error(SimConfig::capacity_message(
                "IoRegistry: outputs of '" + port_records[i].owner_app + "'",
                "max_app_output_number", SimConfig::max_app_output_number));
        }

        plan.output_record_indices[plan.output_record_count] = i;
        plan.output_record_count++;
    }
}

// ---------------- Runtime ----------------

const AppIoPlan& IoRegistry::plan_for(const std::string& app_name) const {
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
// copyable (so copying its bytes is meaningful), and resolve_connection()
// rejects the connection unless source and destination are the exact same
// type. By the time a CopyBinding exists, both ends are the same trivially
// copyable type and `size_bytes` is that type's size.
void IoRegistry::copy_inputs(const AppIoPlan& plan) const {
    for (std::size_t i = 0; i < plan.input_copy_count; i++) {
        const CopyBinding& binding = plan.input_copies[i];
        std::memcpy(binding.destination_ptr, binding.source_ptr, binding.size_bytes);
    }
}

void IoRegistry::stamp_outputs(const AppIoPlan& plan, uint64_t sim_time_usec) {
    for (std::size_t i = 0; i < plan.output_record_count; i++) {
        port_records[plan.output_record_indices[i]].last_write_usec = sim_time_usec;
    }
}

// ---------------- Introspection ----------------

std::string IoRegistry::source_name_of(const PortRecord& record) const {
    if (record.is_connected() == false) {
        return "";
    }

    const PortRecord& source = port_records[record.source_record_index];

    return source.owner_app + "." + source.port_name;
}

// Prints every app's inputs (with their resolved sources) and outputs. This
// is what makes writing an `input_mapping:` block copy-paste rather than
// archaeology
std::string IoRegistry::io_report() const {
    std::ostringstream out;
    out << "[IO REGISTRY] " << declared_app_count << " app(s), " << port_record_count << " port(s)\n";

    for (std::size_t app_index = 0; app_index < declared_app_count; app_index++) {
        const std::string& app_name = declared_app_names[app_index];
        out << "\n" << app_name << "\n";

        for (std::size_t i = 0; i < port_record_count; i++) {
            const PortRecord& record = port_records[i];

            if (record.owner_app != app_name || port_matches(record, PortDirection::Input) == false) {
                continue;
            }

            out << "    <- " << record.port_name << "  [" << record.type_name;
            out << "]";

            if (record.is_connected()) {
                out << "  from " << source_name_of(record);
            } else if (record.is_optional) {
                out << "  (optional, using default)";
            } else {
                out << "  (UNCONNECTED)";
            }
            out << "\n";
        }

        for (std::size_t i = 0; i < port_record_count; i++) {
            const PortRecord& record = port_records[i];

            if (record.owner_app != app_name || port_matches(record, PortDirection::Output) == false) {
                continue;
            }

            out << "    -> " << record.port_name << "  [" << record.type_name;
            out << "]  " << port_kind_to_string(record.kind) << "\n";
        }
    }

    return out.str();
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

bool IoRegistry::port_matches(const PortRecord& record, PortDirection direction) {
    if (direction == PortDirection::Input) {
        return record.kind == PortKind::Subscribe;
    }

    return record.kind != PortKind::Subscribe;
}

// ---------------- Error-message builders ----------------

// Comma-separated list of one app's input or output names, for "here is what
// this app actually declares" errors
std::string IoRegistry::joined_port_names(const std::string& app_name, PortDirection direction) const {
    std::string joined;

    for (std::size_t i = 0; i < port_record_count; i++) {
        const PortRecord& record = port_records[i];

        if (record.owner_app == app_name && port_matches(record, direction)) {
            joined += (joined.empty() ? "" : ", ") + record.port_name;
        }
    }

    return joined.empty() ? "(none)" : joined;
}

// Every output anywhere in the sim carrying `type`, as "app.port" entries.
// Returns an empty string (not "(none)") when there are none, because the
// caller prints a different sentence in that case
std::string IoRegistry::joined_publishers_of_type(const std::type_index& type) const {
    std::string joined;

    for (std::size_t i = 0; i < port_record_count; i++) {
        const PortRecord& record = port_records[i];

        if (port_matches(record, PortDirection::Output) && record.type == type) {
            joined += (joined.empty() ? "" : ", ") + record.owner_app + "." + record.port_name;
        }
    }

    return joined;
}

// ---------------- Typo suggestions ----------------

std::string IoRegistry::closest_app_match(const std::string& query) const {
    std::size_t best_distance = suggestion_threshold(query);
    std::string best_match;

    for (std::size_t i = 0; i < declared_app_count; i++) {
        consider_candidate(declared_app_names[i], query, best_match, best_distance);
    }

    return best_match;
}

std::string IoRegistry::closest_port_match(const std::string& app_name, PortDirection direction,
                                           const std::string& query) const {
    std::size_t best_distance = suggestion_threshold(query);
    std::string best_match;

    for (std::size_t i = 0; i < port_record_count; i++) {
        if (port_records[i].owner_app == app_name && port_matches(port_records[i], direction)) {
            consider_candidate(port_records[i].port_name, query, best_match, best_distance);
        }
    }

    return best_match;
}

std::string IoRegistry::suggestion_text(const std::string& suggestion) {
    if (suggestion.empty()) {
        return "";
    }

    return "\n       did you mean '" + suggestion + "'?";
}

void IoRegistry::consider_candidate(const std::string& candidate, const std::string& query,
                                    std::string& best_match, std::size_t& best_distance) {
    std::size_t distance = edit_distance(query, candidate);

    // A truncated or extended name (one being a prefix of the other) is a
    // likely near-miss even when the raw edit distance is large
    const std::string& shorter = (query.size() < candidate.size()) ? query : candidate;
    const std::string& longer  = (query.size() < candidate.size()) ? candidate : query;

    if (shorter.size() >= 4 && longer.compare(0, shorter.size(), shorter) == 0) {
        distance = 1;
    }

    if (distance < best_distance) {
        best_distance = distance;
        best_match = candidate;
    }
}

std::size_t IoRegistry::suggestion_threshold(const std::string& query) {
    // Only suggest names within a distance that plausibly indicates a typo,
    // scaled a little for longer names
    return std::max<std::size_t>(2, query.size() / 3) + 1;
}

// ---------------- Shared name utilities ----------------

std::string IoRegistry::io_name_violation(const std::string& name) {
    if (name.empty()) {
        return "name is empty";
    }

    for (char character : name) {
        bool valid = (character >= 'a' && character <= 'z') ||
                     (character >= '0' && character <= '9') ||
                     character == '_';

        if (valid == false) {
            std::string shown = (character == ' ') ? std::string("a space") : "'" + std::string(1, character) + "'";
            return "contains " + shown + "; names must use only lowercase letters, digits, and underscores";
        }
    }

    return "";
}

std::size_t IoRegistry::edit_distance(const std::string& word_a, const std::string& word_b) {
    // Comparison length is clamped so the working rows can be fixed-size;
    // suggestion quality is unaffected for any sane port name length
    const std::size_t length_a = std::min(word_a.size(), SimConfig::max_port_name_length);
    const std::size_t length_b = std::min(word_b.size(), SimConfig::max_port_name_length);

    std::array<std::size_t, SimConfig::max_port_name_length + 1> previous_row{};
    std::array<std::size_t, SimConfig::max_port_name_length + 1> current_row{};

    for (std::size_t j = 0; j <= length_b; j++) {
        previous_row[j] = j;
    }

    for (std::size_t i = 1; i <= length_a; i++) {
        current_row[0] = i;

        for (std::size_t j = 1; j <= length_b; j++) {
            std::size_t substitution_cost = (word_a[i - 1] == word_b[j - 1]) ? 0 : 1;
            current_row[j] = std::min({previous_row[j] + 1, current_row[j - 1] + 1, previous_row[j - 1] + substitution_cost});
        }

        std::swap(previous_row, current_row);
    }

    return previous_row[length_b];
}
