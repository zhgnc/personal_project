#include "sim_dispersions.hpp"

void SimDispersions::update_seed(const uint64_t& new_seed) {
    current_seed = new_seed;
    rng.seed(current_seed);
}
    
double SimDispersions::sample_normal(const double& mean, const double& std) {
    assert(std::isfinite(mean) && std::isfinite(std) && "mean and std inputs must be finite numbers!");
    assert(std >= 0 && std::format("Standard deviation must be greater than or equal to zero. Passed value: {:.4f}", std).c_str());
    double random_draw = mean + std * normal_distribution(rng);
    
    return random_draw;
}

double SimDispersions::sample_uniform(const double& lower_bound, const double& upper_bound) {
    assert(std::isfinite(lower_bound) && std::isfinite(upper_bound) && "lower_bound and upper_bound inputs must be finite numbers!");
    assert(lower_bound <= upper_bound && std::format("Lower bound must be less than or equal to upper bound. Passed bounds: [{:.4f}, {:.4f}]", lower_bound, upper_bound).c_str());

    double uniform_range = upper_bound - lower_bound;
    double random_draw   = lower_bound + uniform_range * uniform_distribution(rng); 

    return random_draw;
}

double SimDispersions::sample_uniform_percent(const double& mean, const double& percent_bound) {
    assert(std::isfinite(mean) && std::isfinite(percent_bound) && "mean and percent_bound inputs must be finite numbers!");
    assert(percent_bound >= 0 && std::format("Percent bound must be greater than or equal to zero. Passed value: {:.4f}", percent_bound).c_str());

    double lower_bound = mean * (1.0 - percent_bound / 100.0);
    double upper_bound = mean * (1.0 + percent_bound / 100.0);

    return sample_uniform(lower_bound, upper_bound);
}