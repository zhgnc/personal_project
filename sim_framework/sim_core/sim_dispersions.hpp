#ifndef SIM_DISPERSIONS
#define SIM_DISPERSIONS

#include <cstdint>
#include <random>
#include <cassert> 
#include <format>  

class SimDispersions {
public:
    SimDispersions() = default;

    void update_seed(const uint64_t& new_seed);
    
    double sample_normal(const double& mean, const double& std);
    double sample_uniform(const double& lower_bound, const double& upper_bound);
    double sample_uniform_percent(const double& mean, const double& percent_bound);

private:
    uint64_t current_seed;
    std::mt19937_64 rng;

    std::normal_distribution<double> normal_distribution{0.0, 1.0}; // Mean = 0.0 & std = 1.0
    std::uniform_real_distribution<double> uniform_distribution{0.0, 1.0}; // Bounds = [0,1]
};

#endif