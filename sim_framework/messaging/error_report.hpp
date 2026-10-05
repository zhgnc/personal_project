#ifndef ERROR_REPORT_HPP
#define ERROR_REPORT_HPP

// Collects every problem found during startup and turns them into one report.
//
// The framework never stops at the first bad thing it finds. A config file
// with four mistakes in it should produce four errors in one run, not four
// rebuild-and-rerun cycles. Everything that validates configuration collects
// into one of these and throws once at the end.
//
// Storage is bounded like everything else in the framework: the first
// max_wiring_report_number messages are kept, but every message is counted,
// so the report can say how many were hidden.

#include <array>
#include <cstddef>
#include <stdexcept>
#include <string>

#include "../sim_config.hpp"

// Thrown with the finished report so every startup configuration failure,
// wiring or yaml, surfaces as one exception type
class IoWiringError : public std::runtime_error {
public:
    explicit IoWiringError(const std::string& report) : std::runtime_error(report) {}
};

class ErrorReport {
public:
    void record_error(const std::string& message);

    bool has_errors() const { return total_error_count > 0; }

    // "<heading> N error(s):  (showing first M)" followed by the numbered list
    std::string build_error_text(const std::string& heading) const;

    // Same text, as a thrown IoWiringError. Does nothing when there are no errors
    void throw_if_errors(const std::string& heading) const;

private:
    // True when the array filled up and later messages were dropped
    bool errors_truncated() const { return total_error_count > stored_error_count; }

    std::array<std::string, SimConfig::max_wiring_report_number> errors;
    std::size_t stored_error_count = 0;
    std::size_t total_error_count  = 0;
};

#endif
