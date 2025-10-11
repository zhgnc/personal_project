#ifndef SIM_APP_BASE_INTERFACE_HPP
#define SIM_APP_BASE_INTERFACE_HPP

#include <cstdint>
#include <string>

class DataBus; // Forward declaration to speed up compile time

class SimAppBaseInterface {
public:
    virtual ~SimAppBaseInterface() = default;
    virtual void initialize(const std::string& config_path, DataBus& data_bus) = 0;
    virtual void step() = 0;
};

#endif