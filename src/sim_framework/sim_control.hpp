#ifndef SIMULATION_CONTROL_HPP
#define SIMULATION_CONTROL_HPP

#include <string>

enum class StopReason {
    ReachedConfiguredStopTime = 0,           
    ReachedEndObjective       = 1,
    AlgorithmFailure          = 2,
    SoftwareFailure           = 3,
    HardwareFailure           = 4,
    ConvergenceFailure        = 5,   
    ViolatedConstraint        = 6,  
    Unknown                   = 7             
};

struct SimulationControl {
    SimulationControl() = default;
    virtual ~SimulationControl() = default;

    static std::string stop_reason_to_string(StopReason reason) {
        switch(reason) {
            case StopReason::ReachedConfiguredStopTime: return "Reached Configured Stop Time";
            case StopReason::ReachedEndObjective:       return "Reached End Objective";
            case StopReason::AlgorithmFailure:          return "Algorithm Failure";
            case StopReason::SoftwareFailure:           return "Software Failure";
            case StopReason::HardwareFailure:           return "Hardware Failure";
            case StopReason::ConvergenceFailure:        return "Convergence Failure";
            case StopReason::ViolatedConstraint:        return "Violated Constraint";
            case StopReason::Unknown:                   return "Unknown";
            default:                                                       return "Unknown";
        }
    }

    virtual void stop_sim(StopReason reason = StopReason::Unknown, const std::string& message = "") = 0;
};

#endif
