#ifndef GYRO_MODEL_CONFIG_LOADER_HPP
#define GYRO_MODEL_CONFIG_LOADER_HPP

#include <string>
#include <cmath>
#include <array>
#include <random>

#include "gyro_struct_defs.hpp"
#include "utilities/yaml_utilities.hpp"

gyro_config load_gyro_config(const std::string& config_file_path, const uint64_t& dispersion_seed, const uint64_t& model_seed);

#endif 