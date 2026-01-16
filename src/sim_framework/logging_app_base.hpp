#ifndef LOGGING_APP_BASE_HPP
#define LOGGING_APP_BASE_HPP

#include "../../src/data_logging/logger.hpp"

struct AppLoggingRates {
    double rate_A_hz;
    double rate_B_hz;
    double rate_C_hz;
    double rate_D_hz;
    double rate_E_hz;
};

template<typename DataBusType>
class LoggingAppBase {
public: 
    LoggingAppBase() = default;
    virtual void config_hdf5_with_app_data(Logger& logger, DataBusType& data_bus, AppLoggingRates& rates) = 0;
};

#endif