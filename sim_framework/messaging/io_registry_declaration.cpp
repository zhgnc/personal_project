#include "io_registry.hpp"

void IoRegistry::add_app(const std::string& app_name) {
    throw_if_app_name_is_invalid(app_name);
    throw_if_app_name_is_duplicate(app_name);
    throw_if_app_storage_full(app_name);

    declared_app_names[declared_app_count] = app_name;
    declaring_app_name                     = app_name;
    declaring_app_numerical_index          = declared_app_count;
    declared_app_count++;
}

void IoRegistry::throw_if_app_name_is_invalid(const std::string& app_name) const {
    NameViolation name_violation = NameRules::violation(app_name);

    if (name_violation != NameViolation::None) {
        throw std::logic_error("IoRegistry: app instance name '" + app_name + "' is invalid: " +
                               NameRules::message(name_violation));
    }
}

void IoRegistry::throw_if_app_name_is_duplicate(const std::string& app_name) const {
    if (is_app_declared(app_name) == true) {
        throw std::logic_error("IoRegistry: two apps share the instance name '" + app_name +
                               "': names must be unique because ports are addressed as 'app_name.port_name'");
    }
}

void IoRegistry::throw_if_app_storage_full(const std::string& app_name) const {
    bool app_storage_is_full = declared_app_count >= SimConfig::max_app_number;

    if (app_storage_is_full == true) {
        throw std::logic_error(SimConfig::capacity_message("IoRegistry: declaring app '" + app_name + "'",
                                                           "max_app_number", SimConfig::max_app_number));
    }
}

bool IoRegistry::check_port_name_is_valid(const std::string& port_name) {
    NameViolation name_violation = NameRules::violation(port_name);

    if (name_violation != NameViolation::None) {
        wiring_report.record_error(declaring_app_name + ": port name '" + port_name + "' is invalid: " +
                                   NameRules::message(name_violation));
        return false;
    }

    return true;
}

// An app's inputs and outputs share one namespace, so a name may only appear once
bool IoRegistry::check_port_name_is_unique(const std::string& port_name) {
    bool port_name_already_declared = find_port_index(declaring_app_name, port_name) != port_record_count;

    if (port_name_already_declared == true) {
        wiring_report.record_error(declaring_app_name + ": port '" + port_name + "' is declared more than once");
        return false;
    }

    return true;
}

void IoRegistry::throw_if_port_storage_full(const std::string& port_name) const {
    bool port_storage_is_full = port_record_count >= SimConfig::max_port_number;

    if (port_storage_is_full == true) {
        throw std::logic_error(SimConfig::capacity_message(
            "IoRegistry: declaring port '" + declaring_app_name + "." + port_name + "'",
            "max_port_number", SimConfig::max_port_number));
    }
}

bool IoRegistry::is_app_declared(const std::string& app_name) const {
    std::size_t current_app_number = declared_app_count;
    bool name_matches_another_app  = get_app_index(app_name) != current_app_number;

    return name_matches_another_app;
}

std::size_t IoRegistry::get_app_index(const std::string& app_name) const {
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
