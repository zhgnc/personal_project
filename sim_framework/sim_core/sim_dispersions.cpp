#include "sim_dispersions.hpp"

SimDispersions::SimDispersions(uint64_t init_seed) {
    rng.seed(init_seed);
}

void SimDispersions::reseed(uint64_t new_seed) {
    rng.seed(new_seed);
}
    
double SimDispersions::sample_normal(double mean, double std) {
    assert(std::isfinite(mean) && "Mean must be finite");
    assert(std::isfinite(std)  && "Standard deviation must be finite");
    assert(std >= 0.0          && "Standard deviation must be >= 0.0");

    double random_draw = mean + std * normal_distribution(rng);
    
    return random_draw;
}

double SimDispersions::sample_uniform(double lower_bound, double upper_bound) {
    assert(std::isfinite(lower_bound) && "Lower bound must be finite");
    assert(std::isfinite(upper_bound) && "Upper bound must be finite");
    assert(lower_bound <= upper_bound && "Lower bound must be <= upper bound");

    double uniform_range = upper_bound - lower_bound;
    double random_draw   = lower_bound + uniform_range * uniform_distribution(rng); 

    return random_draw;
}

double SimDispersions::sample_uniform_percent(double mean, double percent_bound) {
    assert(std::isfinite(mean)          && "Mean must be finite");
    assert(std::isfinite(percent_bound) && "Percent bound must be finite");
    assert(percent_bound >= 0.0         && "Percent bound must be >= 0.0");

    double lower_bound = mean * (1.0 - percent_bound / 100.0);
    double upper_bound = mean * (1.0 + percent_bound / 100.0);

    if (mean < 0.0) {
        // For negative means the percentage scaling reverses the ordering
        // of the bounds. Example: mean = -100 and percent_bound = 10%
        // produces [-90, -110], so the bounds must be swapped before calling
        // sample_uniform or else the assert(lower <= upper) will catch
        std::swap(lower_bound, upper_bound);
    }

    return sample_uniform(lower_bound, upper_bound);
}