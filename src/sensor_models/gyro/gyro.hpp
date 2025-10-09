#ifndef GYRO_HPP
#define GYRO_HPP

#include "../../math_utilities/math.hpp"
#include "struct_defs.hpp"

class gyro 
{
public:
    gyro();
    gyro(const std::string& config_file);

    void initialize();
    void get_inputs(); 
    void execute(); 
    void set_outputs();

    double frequency;

private:
    gyro_config config;
    double arw_1_sigma; 
    double rrw_1_sigma; 
    double sf_1_sigma; 
    double misalign_1_sigma;
};

#endif