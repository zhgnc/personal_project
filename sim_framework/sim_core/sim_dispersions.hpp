#ifndef SIM_DISPERSIONS
#define SIM_DISPERSIONS

#include <cmath>
#include <cstdint>
#include <random>
#include <cassert> 
#include <utility>

class SimDispersions {
public:
    explicit SimDispersions(uint64_t init_seed);

    void reseed(uint64_t new_seed);  
    
    double sample_normal(double mean, double std);
    double sample_uniform(double lower_bound, double upper_bound);
    double sample_uniform_percent(double mean, double percent_bound);

private:
    std::mt19937_64 rng;

    std::normal_distribution<double> normal_distribution{0.0, 1.0}; // Mean = 0.0 & std = 1.0
    std::uniform_real_distribution<double> uniform_distribution{0.0, 1.0}; // Bounds = [0,1]
};

#endif