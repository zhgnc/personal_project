#ifndef TEST_DATABUS_HPP
#define TEST_DATABUS_HPP

struct TestDataBus {
    struct App1Data {
        int counter = 0;
    };

    struct App2Data {
        int counter = 0;
    };


    App1Data app_1_data;
    App2Data app_2_data;
};

#endif