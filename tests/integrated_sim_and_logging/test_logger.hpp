#ifndef TEST_LOGGER_HPP
#define TEST_LOGGER_HPP

#include <memory>
#include <iostream>
#include <string>
#include <format>

#include "../../../src/sim_framework/logging_app_base.hpp"
#include "../../../tests/integrated_sim_and_logging/test_data_bus.hpp"

class TestLogger : public LoggingAppBase<TestDataBus> {
public:
    using LoggingAppBase<TestDataBus>::LoggingAppBase;

    void configure_hdf5_file() override {
        double record_freq_hz = 10.0;

        logger.add_group("test_group");
        logger.add_dataset<int>("counter", "test_group", data_bus->app_1_data.counter, record_freq_hz);
        // logger.print_file_tree();
    };
};

#endif