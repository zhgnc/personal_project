#ifndef TEST_LOGGER_2_HPP
#define TEST_LOGGER_2_HPP

#include <memory>
#include <iostream>
#include <string>
#include <format>

#include "../../../src/sim_framework/logging_app_base.hpp"
#include "../../../tests/integrated_sim_and_logging/common_test_code/test_data_bus.hpp"

class TestLogger2 : public LoggingAppBase<TestDataBus> {
public:
    using LoggingAppBase<TestDataBus>::LoggingAppBase;

    void configure_hdf5_logging(LoggerFacade& logger, const TestDataBus& data_bus, const AppLoggingRates& rates) override {
        logger.add_group("app_2");
        logger.add_group("test_group");

        logger.add_dataset<int>("counter",             "app_2", data_bus.app_2_data.counter,        rates.rate_C_hz);
        logger.add_dataset<double>("sim_time_sec",     "app_2", data_bus.app_2_data.sim_time_sec,   rates.rate_C_hz);
        logger.add_dataset<uint64_t>("sim_step_count", "app_2", data_bus.app_2_data.sim_step_count, rates.rate_C_hz);
        logger.add_dataset<double>("sim_rate_hz",      "app_2", data_bus.app_2_data.sim_rate_hz,    rates.rate_C_hz);
        logger.add_dataset<double>("sim_dt_sec",       "app_2", data_bus.app_2_data.sim_dt_sec,     rates.rate_C_hz);

        logger.write_attribute<uint64_t>("test_group", "test_attribute_write_in_logger", 12345);
    };
};

#endif