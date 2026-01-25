#ifndef LOGGING_APP_BASE_HPP
#define LOGGING_APP_BASE_HPP

#include "../../src/data_logging/logger.hpp"
#include "../../src/sim_framework/logging_facade.hpp"

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
    virtual void configure_hdf5_logging(LoggerFacade& logger, const DataBusType& data_bus, const AppLoggingRates& rates) = 0;
};

#endif