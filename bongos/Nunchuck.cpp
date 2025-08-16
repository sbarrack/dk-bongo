#include "Nunchuck.hpp"

#define WRITE_COOLDOWN  50
#define READ_COOLDOWN   300

// I2C clock speeds (bits/sec)
#define F_BASE      100000
#define F_FAST      400000  // Default for Wii
#define F_FASTER    1000000
// I2C status codes
#define ACK             0
#define NACK_OVERFLOW   1
#define NACK_ADDR       2
#define NACK_VAL        3
#define NACK            4
// Addresses
#define SLAVE   0x52

uint8_t Nunchuck::write(uint8_t addr, uint8_t val) {
    uint8_t status;

    i2c.beginTransmission(SLAVE);
    i2c.write(addr);
    if (val == 0) {
        cooldown = READ_COOLDOWN;
        isPolling = true;
    } else {
        cooldown = WRITE_COOLDOWN;
        i2c.write(val);
    }
    status = i2c.endTransmission();
    dt = 0;
    if (status == NACK) isEncrypted = true;

    return status;
}

Nunchuck::Nunchuck(TwoWire &i2c) : i2c(i2c) {}

void Nunchuck::setup() {
    i2c.setClock(F_FASTER);
    i2c.begin();    // Master
}

void Nunchuck::loop() {
    if (cooldown > 0) {
        // Doing this cooldown locking mechanism lets me interpolate polling
        // the other controllers without blocking execution with delay() nor
        // complex parallelization :)
        unsigned long delta = dt;
        dt = 0;

        cooldown = delta > cooldown ? 0 : cooldown - delta;

        return;
    }

    if (isEncrypted && write(0xf0, 0x55) == ACK) {
        isEncrypted = false;

        return; // Let cooldown happen before polling.
    }

    if (isPolling) {
        Packet p;

        // I don't do `while (Wire.available() > 0)` because the controller
        // ALWAYS has bytes available. They're just all null (0xFF) until it
        // has had enough time to write the input data. Also, I can't get an
        // index that way.
        //
        // requestFrom() returns the number of bytes, so I can store that once
        // in the index initializer and avoid doing a function call every
        // iteration. I do have to initialize `raw` backwards, but it hits the
        // right balance of reliability and performance for me.
        for (int i = Wire.requestFrom(SLAVE, sizeof(p.raw)) - 1; i >= 0; i--) {
            p.raw[i] = Wire.read();
        }
        c = !p.c;
        z = !p.z;
        x = p.x;
        y = p.y;
        ax = p.ax << 2 | p.ax2;
        ay = p.ay << 2 | p.ay2;
        az = p.az << 2 | p.az2;

        isPolling = false;

        return;
    }
    write(0);
}

bool Nunchuck::getC() const {
    return c;
}

bool Nunchuck::getZ() const {
    return z;
}

uint8_t Nunchuck::getX() const {
    return x;
}

uint8_t Nunchuck::getY() const {
    return y;
}

uint16_t Nunchuck::getAx() const {
    return ax;
}

uint16_t Nunchuck::getAy() const {
    return ay;
}

uint16_t Nunchuck::getAz() const {
    return az;
}
