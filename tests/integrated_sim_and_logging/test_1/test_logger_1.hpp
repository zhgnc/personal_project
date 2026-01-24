#ifndef TEST_LOGGER_HPP
#define TEST_LOGGER_HPP

#include <memory>
#include <iostream>
#include <string>
#include <format>

#include "../../../src/sim_framework/logging_app_base.hpp"
#include "../../../tests/integrated_sim_and_logging/common_test_code/test_data_bus.hpp"

class TestLogger1 : public LoggingAppBase<TestDataBus> {
public:
    using LoggingAppBase<TestDataBus>::LoggingAppBase;

    void config_hdf5_with_app_data(Logger& logger, TestDataBus& data_bus, AppLoggingRates& rates) override {
        logger.add_group("test_group");
        logger.add_dataset<int>("counter", "test_group", data_bus.app_1_data.counter, rates.rate_A_hz);
    };
};

#endif