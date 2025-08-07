#pragma once
#include <Arduino.h>

union gcStatus {
    uint8_t raw[3];

    struct {
        uint16_t device;

        union {
            uint8_t status;

            struct {
                uint8_t status0 : 3;
                uint8_t rumble : 1;
                uint8_t status1 : 4;
            };
        };
    };
};

#ifdef __cplusplus
extern "C" {
#endif
    void gcSend(const uint8_t* buff, uint8_t len, volatile uint8_t* modePort, volatile uint8_t* outPort, uint8_t bitMask) __attribute__((noinline));
    uint8_t gcGet(uint8_t* buff, uint8_t len, volatile uint8_t* modePort, volatile uint8_t* outPort, volatile uint8_t * inPort, uint8_t bitMask)__attribute__((noinline));
    uint8_t gcSendGet(const uint8_t pin, uint8_t* command, const uint8_t commandLen, uint8_t* report, const uint8_t reportLen)__attribute__((noinline));
#ifdef __cplusplus
}
#endif
