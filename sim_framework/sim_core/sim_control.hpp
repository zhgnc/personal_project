#ifndef SIMULATION_CONTROL_HPP
#define SIMULATION_CONTROL_HPP

#include "../data_logging/logger.hpp"
#include "sim_dispersions.hpp"

#include <string>
#include <functional>

enum class StopType {
    AfterApp   = 0,
    AfterCycle = 1,
    NoStop     = 2,
};

enum class StopReason {
    ReachedConfiguredStopTime = 0,           
    ReachedEndObjective       = 1,
    AlgorithmFailure          = 2,
    SoftwareFailure           = 3,
    HardwareFailure           = 4,
    NumericalFailure          = 5,
    ConvergenceFailure        = 6,   
    ViolatedConstraint        = 7,  
    NotSpecified              = 8             
};

inline std::string stop_reason_to_string(StopReason reason) {
    switch(reason) {
        case StopReason::ReachedConfiguredStopTime: return "Reached Configured Stop Time";
        case StopReason::ReachedEndObjective:       return "Reached End Objective";
        case StopReason::AlgorithmFailure:          return "Algorithm Failure";
        case StopReason::SoftwareFailure:           return "Software Failure";
        case StopReason::HardwareFailure:           return "Hardware Failure";
        case StopReason::NumericalFailure:          return "Numerical Failure";
        case StopReason::ConvergenceFailure:        return "Convergence Failure";
        case StopReason::ViolatedConstraint:        return "Violated Constraint";
        case StopReason::NotSpecified:              return "Not Specified";
        default:                                    return "Error Reporting StopReason Enum!";
    }
}

inline std::string stop_type_to_string(StopType type) {
    switch(type) {
        case StopType::AfterApp:   return "After This App";
        case StopType::AfterCycle: return "After This Cycle";
        case StopType::NoStop:     return "No Stop (Completed Whole Sim)";
        default:                   return "Error Reporting StopType Enum!";
    }
}

class SimControl {
public:
    struct AccessibleSimData {
        double   current_sim_time_sec;
        double   sim_dt_sec;
        double   sim_rate_hz;
        uint64_t sim_step_count;
    };

    SimControl(
        const AccessibleSimData& sim_data_ref,
        std::function<void(StopType, StopReason, const std::string&)> end_sim_func,
        std::function<uint64_t()> next_seed_func,
        Logger& logger_ref,
        uint64_t init_seed
    )
    : sim_data(sim_data_ref),
      end_sim_callback(end_sim_func),
      get_seed_callback(next_seed_func),
      wrapped_logger(logger_ref),
      dispersions(init_seed)
    {}

    const AccessibleSimData& public_sim_data() const{
        return sim_data;
    };

    void end_sim(const StopType& type, const StopReason& reason = StopReason::NotSpecified, const std::string& message = "") {
        end_sim_callback(type, reason, message);
    };

    uint64_t get_seed() {
        return get_seed_callback();
    };

    template <typename T>
    void write_attribute(const std::string& group_path, const std::string& attribute_name, const T& value) {
        // App-facing attribute writes target groups, and the group is created
        // on first use: apps write config attributes during configure_model(),
        // before the recorder has built the file structure
        wrapped_logger.add_group(group_path);
        wrapped_logger.write_attribute(group_path, attribute_name, value);
    };

    double sample_normal(double mean, double std) {
        return dispersions.sample_normal(mean, std);
    }

    double sample_uniform(double lower_bound, double upper_bound) {
        return dispersions.sample_uniform(lower_bound, upper_bound);
    };

    double sample_uniform_percent(double mean, double percent) {
        return dispersions.sample_uniform_percent(mean, percent);
    };


protected:
    friend class SimDataLogger; // Needs `_to_string` functions for logging

    const AccessibleSimData& sim_data;
    std::function<void(StopType, StopReason, const std::string&)> end_sim_callback;
    std::function<uint64_t()> get_seed_callback;
    Logger& wrapped_logger;

private:
    SimDispersions dispersions;
};

#endif
