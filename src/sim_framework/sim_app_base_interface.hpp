#ifndef SIM_APP_BASE_INTERFACE_HPP
#define SIM_APP_BASE_INTERFACE_HPP

#include <cstdint>

class SimAppBaseInterface {
public:
    virtual ~SimAppBaseInterface() = default;
    virtual void initialize() = 0;
    virtual void step() = 0;
};

#endif