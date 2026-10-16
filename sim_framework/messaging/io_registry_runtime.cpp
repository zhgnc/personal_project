#include "io_registry.hpp"

#include <cstring>

// Called once per app at setup and each app keeps the plan generated 
// here and passes it back to the two functions below every frame
const AppIoInfo& IoRegistry::plan_for(const std::string& app_name) const {
    std::size_t app_index = get_app_index(app_name);

    // get_app_index() returns declared_app_count when the name is not found,
    // which is a readable index into app_plans, so it has to be caught here
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
// copyable, and check_types_match() rejects the connection unless source
// and destination are the exact same type
void IoRegistry::copy_data(const AppIoInfo& plan) const {
    for (std::size_t i = 0; i < plan.input_copy_count; i++) {
        const CopyBinding& binding = plan.input_copies[i];

        std::memcpy(binding.destination_ptr, binding.source_ptr, binding.size_bytes);
    }
}

void IoRegistry::time_stamp_outputs(const AppIoInfo& plan, uint64_t sim_time_usec) {
    for (std::size_t i = 0; i < plan.output_record_count; i++) {
        port_records[plan.output_record_indices[i]].last_write_usec = sim_time_usec;
    }
}
