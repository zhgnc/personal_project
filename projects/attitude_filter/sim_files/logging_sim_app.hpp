#ifndef LOGGER_SIM_INTERFACE_HPP
#define LOGGER_SIM_INTERFACE_HPP

#include <memory>
#include <iostream>
#include <string>
#include <format>

#include "../../../sim_framework/sim_includes.hpp"
#include "../../../projects/attitude_filter/sim_files/data_bus.hpp"

class LoggingSimApp : public LoggingAppBase<DataBus> {
public:
    using LoggingAppBase<DataBus>::LoggingAppBase;

    void configure_hdf5_logging(LoggerFacade& logger, const DataBus& data_bus, const AppLoggingRates& rates) override {
        // Dynamics
        logger.add_group("fake_dynamics");

        logger.add_dataset("quat",             "fake_dynamics", data_bus.fake_dynamics_outputs.quat,             rates.rate_A_hz);
        logger.add_dataset("rot_vec_attitude", "fake_dynamics", data_bus.fake_dynamics_outputs.rot_vec_attitude, rates.rate_A_hz);
        logger.add_dataset("body_rates",       "fake_dynamics", data_bus.fake_dynamics_outputs.body_rates,       rates.rate_A_hz);

        
        // Gyro
        logger.add_group("gyro");

        logger.add_dataset("gyro_meas_valid_flag",    "gyro", data_bus.gyro_outputs.measurement_valid,       rates.rate_B_hz);
        logger.add_dataset("meas_delta_angles",       "gyro", data_bus.gyro_outputs.measured_delta_angles,   rates.rate_B_hz);
        logger.add_dataset("total_delta_angle_error", "gyro", data_bus.gyro_outputs.total_delta_angle_error, rates.rate_B_hz);
        logger.add_dataset("angle_biases",            "gyro", data_bus.gyro_outputs.angle_biases,            rates.rate_B_hz);
        logger.add_dataset("scale_factors",           "gyro", data_bus.gyro_outputs.scale_factors,           rates.rate_B_hz);
        logger.add_dataset("misalignments",           "gyro", data_bus.gyro_outputs.misalignments,           rates.rate_B_hz);
        logger.add_dataset("seed",                    "gyro", data_bus.gyro_outputs.seed,                    rates.rate_B_hz);


        // Star Tracker
        logger.add_group("star_tracker");

        logger.add_dataset("measurement_valid",            "star_tracker", data_bus.star_tracker_outputs.measurement_valid,            rates.rate_C_hz);
        logger.add_dataset("q_j2000_to_star_tracker_meas", "star_tracker", data_bus.star_tracker_outputs.q_j2000_to_star_tracker_meas, rates.rate_C_hz);
        logger.add_dataset("measurement_error_rad",        "star_tracker", data_bus.star_tracker_outputs.measurement_error_rad,        rates.rate_C_hz);
        logger.add_dataset("seed",                         "star_tracker", data_bus.star_tracker_outputs.seed,                         rates.rate_C_hz);
    
        // Attitude filter
        logger.add_group("attitude_filter");

        logger.add_dataset("q_j2000_to_body_est",              "attitude_filter", data_bus.attitude_filter_outputs.q_j2000_to_body_est,              rates.rate_D_hz);
        logger.add_dataset("corrected_gyro_delta_thetas",      "attitude_filter", data_bus.attitude_filter_outputs.corrected_gyro_delta_thetas,      rates.rate_D_hz);
        logger.add_dataset("est_gyro_to_st_misalignments_rad", "attitude_filter", data_bus.attitude_filter_outputs.est_gyro_to_st_misalignments_rad, rates.rate_D_hz);
        logger.add_dataset("est_gyro_scale_factors",           "attitude_filter", data_bus.attitude_filter_outputs.est_gyro_scale_factors,           rates.rate_D_hz);
        logger.add_dataset("rot_vec_residual",                 "attitude_filter", data_bus.attitude_filter_outputs.rot_vec_residual,                 rates.rate_D_hz);
        logger.add_dataset("diagonal_covariance",              "attitude_filter", data_bus.attitude_filter_outputs.covariance_diagonals,             rates.rate_D_hz);

        logger.add_dataset("rot_vec_error",                    "attitude_filter", data_bus.attitude_filter_performance.rot_vec_error,                rates.rate_D_hz);
        logger.add_dataset("bias_error",                       "attitude_filter", data_bus.attitude_filter_performance.bias_error,                   rates.rate_D_hz);
        logger.add_dataset("misalignment_error",               "attitude_filter", data_bus.attitude_filter_performance.misalignment_error,           rates.rate_D_hz);
        logger.add_dataset("scale_factor_error",               "attitude_filter", data_bus.attitude_filter_performance.scale_factor_error,           rates.rate_D_hz);
    };
};

#endif