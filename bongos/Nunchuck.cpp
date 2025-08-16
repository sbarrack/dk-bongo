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

        for (int i = Wire.requestFrom(SLAVE, sizeof(p.raw)) - 1; i >= 0; i--) {
            p.raw[i] = Wire.read();
        }
        isPolling = false;
        // FIXME Transform raw

        return;
    }
    write(0);
}
