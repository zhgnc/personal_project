#ifndef SIMULATION_CONTROL_HPP
#define SIMULATION_CONTROL_HPP

#include <string>

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

class SimulationControl {
public:
    SimulationControl() = default;

    struct AccessibleSimData {
        double   current_sim_time_sec;
        double   sim_dt_sec;
        double   sim_rate_hz;
        uint64_t sim_step_count;
    };

    virtual const AccessibleSimData& public_sim_data() const = 0;
    virtual void end_sim(const StopType& type, const StopReason& reason = StopReason::NotSpecified, const std::string& message = "") = 0;
    virtual uint64_t get_next_seed() = 0;


protected:
    friend class SimDataLogger; // Needs `_to_string` functions for logging

    static std::string stop_reason_to_string(StopReason reason) {
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

    static std::string stop_type_to_string(StopType type) {
        switch(type) {
            case StopType::AfterApp:   return "After This App";
            case StopType::AfterCycle: return "After This Cycle";
            case StopType::NoStop:     return "No Stop (Completed Whole Sim)";
            default:                   return "Error Reporting StopType Enum!";
        }
    }
};

#endif
