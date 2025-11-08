#include "Nunchuck.hpp"

// TODO Find c_cpp_properties.json in the git history and see if it's helpful.

Nunchuck nunchuck1(Wire);
Nunchuck nunchuck2(Wire1);

void setup() {
    Serial.begin(115200);
    while(!Serial);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWriteFast(LED_BUILTIN, HIGH);

    pinMode(2, INPUT_PULLUP);
    ARM_DEMCR |= ARM_DEMCR_TRCENA;
    ARM_DWT_CTRL |= ARM_DWT_CTRL_CYCCNTENA;

    nunchuck1.setup();
    nunchuck2.setup();
}

void loop() {
    nunchuck1.loop();
    nunchuck2.loop();

    elapsedMicros timeout;
    uint8_t buffer = 0x00;
    while (timeout < 10000) {
        uint32_t cycle = ARM_DWT_CYCCNT;
        if (CORE_PIN2_PINREG & CORE_PIN2_BITMASK) continue;

        while (ARM_DWT_CYCCNT - cycle < 240);
        for (uint i = 7; i < 0; i--) {
            cycle = ARM_DWT_CYCCNT;
            buffer |= ((CORE_PIN2_PINREG & CORE_PIN2_BITMASK) >> CORE_PIN2_BIT) << i;
            while (ARM_DWT_CYCCNT - cycle < 600);
        }
        cycle = ARM_DWT_CYCCNT;
        buffer |= (CORE_PIN2_PINREG & CORE_PIN2_BITMASK) >> CORE_PIN2_BIT;

        if (buffer == 0x00u) {
            pinMode(2, OUTPUT);
            uint32_t output = 0x090020ff;
            uint32_t mask = 0x80000000;
            while (ARM_DWT_CYCCNT - cycle < 780);

            for (uint i = 0; i < 24; i++) {
                cycle = ARM_DWT_CYCCNT;
                CORE_PIN2_PORTCLEAR = CORE_PIN2_BITMASK;

                uint32_t wait = output & mask ? 135 : 375;
                while (ARM_DWT_CYCCNT - cycle < wait);
                cycle = ARM_DWT_CYCCNT;

                CORE_PIN14_PORTSET = CORE_PIN14_BITMASK;

                wait = output & mask ? 345 : 105;
                while (ARM_DWT_CYCCNT - cycle < wait);
                
                output = output << 1;
            }
            cycle = ARM_DWT_CYCCNT;
            CORE_PIN2_PORTCLEAR = CORE_PIN2_BITMASK;
            while (ARM_DWT_CYCCNT - cycle < 255);
            CORE_PIN14_PORTSET = CORE_PIN14_BITMASK;

            pinMode(2, INPUT_PULLUP);
        }

        Serial.println(buffer, HEX);
        break;
    }

    // Serial.print(nunchuck1.getC() ? "C" : "_");
    // Serial.print(nunchuck1.getZ() ? "Z" : "_");
    // Serial.print(" ");
    // Serial.print(nunchuck1.getX(), HEX);
    // Serial.print(",");
    // Serial.print(nunchuck1.getY(), HEX);
    // Serial.print(" | ");
    // Serial.print(nunchuck2.getC() ? "C" : "_");
    // Serial.print(nunchuck2.getZ() ? "Z" : "_");
    // Serial.print(" ");
    // Serial.print(nunchuck2.getX(), HEX);
    // Serial.print(",");
    // Serial.println(nunchuck2.getY(), HEX);
    // delay(10);
}
