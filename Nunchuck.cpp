#include "Nunchuck.hpp"

#define ACK             0
#define NACK_OVERFLOW   1
#define NACK_ADDR       2
#define NACK_VAL        3
#define NACK            4

#define SLAVE   0x52

#define WRITE_COOLDOWN  50

#define DECRYPT1    0
#define DECRYPT2    1
#define IDENTIFY    2

uint8_t Nunchuck::write(uint8_t addr, uint8_t val = 0) {
    i2c.beginTransmission(SLAVE);
    i2c.write(addr);
    i2c.write(val);
    uint8_t status = i2c.endTransmission();
    dt = 0;
    if (status == NACK) initStage = DECRYPT1;
    cooldown = WRITE_COOLDOWN;
    return status;
}

Nunchuck::Nunchuck(TwoWire &i2c) : i2c(i2c) {}

void Nunchuck::setup() {
    i2c.setClock(400000);   // Fast mode
    i2c.begin();
}

void Nunchuck::loop() {
    if (cooldown > 0) {
        unsigned long delta = dt;
        dt = 0;
        cooldown = delta > cooldown ? 0 : cooldown - delta;
        return;
    }

    if (initStage == DECRYPT2) {
        uint8_t status = write(0xfb, 0);
        if (status == ACK) initStage = IDENTIFY;
        else if (status == NACK) initStage = DECRYPT1;
        return;
    }

    // First byte = the final byte of the address for a Wii remote's attachment register
    // Second byte = the value to write at that address
    if (initStage == DECRYPT1 && write(0xf0, 0x55) == ACK) {
        initStage = DECRYPT2;
    }
}
