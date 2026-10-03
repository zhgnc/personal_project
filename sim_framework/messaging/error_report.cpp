#include "error_report.hpp"

#include <sstream>

void ErrorReport::record_error(const std::string& message) {
    if (stored_error_count < errors.size()) {
        errors[stored_error_count] = message;
        stored_error_count++;
    }

    // Counted even when it was not stored, so the report can say how many
    // messages are missing
    total_error_count++;
}

void ErrorReport::record_warning(const std::string& message) {
    if (stored_warning_count < warnings.size()) {
        warnings[stored_warning_count] = message;
        stored_warning_count++;
    }

    total_warning_count++;
}

std::string ErrorReport::build_error_text(const std::string& heading) const {
    std::ostringstream report;

    report << heading << " " << total_error_count << " error(s):";

    if (errors_truncated()) {
        report << "  (showing first " << stored_error_count << ")";
    }
    report << "\n";

    for (std::size_t i = 0; i < stored_error_count; i++) {
        report << "\n  " << (i + 1) << ". " << errors[i] << "\n";
    }

    return report.str();
}

void ErrorReport::throw_if_errors(const std::string& heading) const {
    if (has_errors() == false) {
        return;
    }

    throw IoWiringError(build_error_text(heading));
}
