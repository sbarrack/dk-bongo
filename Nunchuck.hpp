#pragma once
#include <Arduino.h>
#include <Wire.h>

class Nunchuck {
    TwoWire &i2c;
    elapsedMicros dt;
    int initStage           = 0;
    unsigned long cooldown  = 0;

    uint8_t write(uint8_t *data, size_t len);

public:
    Nunchuck(TwoWire &i2c);
    void setup();
    void loop();
};