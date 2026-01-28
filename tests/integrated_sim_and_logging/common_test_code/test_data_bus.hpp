#ifndef TEST_DATABUS_HPP
#define TEST_DATABUS_HPP

struct TestDataBus {
    struct App1Data {
        int counter = 0;
    };

    struct App2Data {
        int counter = 0;
        double sim_time_sec = 0;
        uint64_t sim_step_count = 0;
        double sim_dt_sec = 0;
        double sim_rate_hz = 0;
    };
    

    App1Data app_1_data;
    App2Data app_2_data;
};

#endif