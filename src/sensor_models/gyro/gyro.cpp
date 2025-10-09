#ifndef GYRO_CPP
#define GYRO_CPP

#include "gyro.hpp"
#include <yaml-cpp/yaml.h>

gyro::gyro() {
  arw_1_sigma      = config.angle_random_walk_1_sigma;
  rrw_1_sigma      = config.rate_random_walk_1_sigma;
  sf_1_sigma       = config.scale_factor_1_sigma;
  misalign_1_sigma = config.misalignment_1_sigma;
  frequency        = config.rate_hz;
}

gyro::gyro(const std::string& config_file) {
  YAML::Node config_data = YAML::LoadFile(config_file); 
  
  arw_1_sigma      = config_data["angle_random_walk_1_sigma"].as<double>();
  rrw_1_sigma      = config_data["rate_random_walk_1_sigma"].as<double>();
  sf_1_sigma       = config_data["scale_factor_1_sigma"].as<double>();
  misalign_1_sigma = config_data["misalignment_1_sigma"].as<double>();
  frequency        = config_data["rate_hz"].as<double>();
}

void gyro::initialize() {

};

#endif