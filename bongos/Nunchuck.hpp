#pragma once
#include <Arduino.h>
#include <Wire.h>

/** https://wiibrew.org/wiki/Wiimote/Extension_Controllers */
class Nunchuck {
    union Packet {
        uint8_t raw[6];

        struct {
            // Axes
            // Joystick
            uint8_t x;
            uint8_t y;
            // Accelerometer significant bits
            uint8_t ax;
            uint8_t ay;
            uint8_t az;
            
            // Teensy is little-endian, so LSB-first.
            // Buttons (idle-high; i.e. 0 = pressed, 1 = not pressed)
            uint8_t z   : 1;
            uint8_t c   : 1;
            // Accelerometer insignificant bits
            uint8_t ax2 : 2;
            uint8_t ay2 : 2;
            uint8_t az2 : 2;
        };
    };
    
    TwoWire &i2c;
    elapsedMicros dt;
    bool isEncrypted        = true;
    bool isPolling          = false;
    unsigned long cooldown  = 0;

    uint8_t write(uint8_t addr, uint8_t val = 0);

public:
    Nunchuck(TwoWire &i2c);
    void setup();
    void loop();
};