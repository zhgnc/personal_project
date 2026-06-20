#ifndef ATTITUDE_FILTER_CONFIG_LOADER_HPP
#define ATTITUDE_FILTER_CONFIG_LOADER_HPP

#include <string>
#include <cmath>
#include <array>

#include "yaml-cpp/yaml.h"

#include "attitude_filter_struct_defs.hpp"
#include "utilities/yaml_utilities.hpp"

AttitudeFilterConfig load_attitude_filter_config(const std::string& config_file_path);

#endif 