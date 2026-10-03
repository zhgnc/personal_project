#ifndef IO_DECLARATIONS_HPP
#define IO_DECLARATIONS_HPP

// What a sim app is handed inside declare_io().
//
// This class exists so that when users declare their app's inputs and outputs
// they only have access to the four declaration calls and not to the rest of
// IoRegistry, which belongs to the framework.

#include <string>

#include "io_registry.hpp"

class IoDeclarations {
public:
    explicit IoDeclarations(IoRegistry& registry) : wrapped_registry(registry) {}

    template<typename T>
    void sub(const std::string& port_name, T& destination) {
        wrapped_registry.sub(port_name, destination);
    }

    template<typename T>
    void sub_optional(const std::string& port_name, T& destination, const T& default_value) {
        wrapped_registry.sub_optional(port_name, destination, default_value);
    }

    template<typename T>
    void tlm(const std::string& port_name, T& source) {
        wrapped_registry.tlm(port_name, source);
    }

    template<typename T>
    void tlm_debug(const std::string& port_name, T& source) {
        wrapped_registry.tlm_debug(port_name, source);
    }

private:
    IoRegistry& wrapped_registry;
};

#endif
