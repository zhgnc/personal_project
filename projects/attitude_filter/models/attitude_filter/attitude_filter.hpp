#ifndef ATTITUDE_FILTER_HPP
#define ATTITUDE_FILTER_HPP

#include "string"

#include "attitude_filter_struct_defs.hpp"

class AttitudeFilter
{
public:
    AttitudeFilter() = default;
    AttitudeFilter(std::string path_to_config);

    void run();

    AttitudeFilterInputs inputs;
    AttitudeFilterOutputs outputs;

private:

};


#endif 