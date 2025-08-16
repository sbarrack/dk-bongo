#pragma once
#include <Arduino.h>
#include <Wire.h>

/** https://wiibrew.org/wiki/Wiimote/Extension_Controllers */
class Nunchuck {
    union Packet {
        uint8_t raw[6];

        // Bytes are in reverse-order because I receive them that way in
        // loop(). I explain why in there.
        struct {
            // Teensy is little-endian, so LSB-first.
            // Buttons (idle-high; i.e. 0 = pressed, 1 = not pressed)
            uint8_t z   : 1;
            uint8_t c   : 1;
            // Accelerometer insignificant bits
            uint8_t ax2 : 2;
            uint8_t ay2 : 2;
            uint8_t az2 : 2;

            // Axes
            // Accelerometer significant bits
            uint8_t az;
            uint8_t ay;
            uint8_t ax;
            // Joystick
            uint8_t y;
            uint8_t x;
        };
    };
    
    TwoWire &i2c;
    elapsedMicros dt;
    bool isEncrypted        = true;
    bool isPolling          = false;
    unsigned long cooldown  = 0;
    bool c;
    bool z;
    uint8_t x;
    uint8_t y;
    uint16_t ax;
    uint16_t ay;
    uint16_t az;

    uint8_t write(uint8_t addr, uint8_t val = 0);

public:
    Nunchuck(TwoWire &i2c);
    void setup();
    void loop();
    bool getC() const;
    bool getZ() const;
    uint8_t getX() const;
    uint8_t getY() const;
    uint16_t getAx() const;
    uint16_t getAy() const;
    uint16_t getAz() const;
};
