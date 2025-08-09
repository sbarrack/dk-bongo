#include <Arduino.h>
#include <Wire.h>
// #include "libs/i2c_t3/i2c_t3.h"

// https://wiibrew.org/wiki/Wiimote/Extension_Controllers

#define I2C_SLAVE_ADDRESS_WII_ATTACHMENT 0x52

const uint8_t CONTROLLER_ID_NUNCHUCK[6] = {0x00, 0x00, 0xA4, 0x20, 0x00, 0x00};

struct NunchuckPacket {
    uint8_t joystickXAxis;
    uint8_t joystickYAxis;
    uint8_t accelerometerXAxisSignificantBits;
    uint8_t accelerometerYAxisSignificantBits;
    uint8_t accelerometerZAxisSignificantBits;
    
    uint8_t accelerometerZAxisInsignificantBits : 2;
    uint8_t accelerometerYAxisInsignificantBits : 2;
    uint8_t accelerometerXAxisInsignificantBits : 2;
    uint8_t c : 1;
    uint8_t z : 1;
};

void setup() {
    Wire.begin();
    Wire1.begin();
    // Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, 400000);
    // Wire.setDefaultTimeout(200000);
    
    Serial.begin(115200);
    while (!Serial);
    delay(3000); // screen /dev/ttyACM0 115200
}

void loop() {
    testConnection(Wire, 0);
    testConnection(Wire1, 1);
    
    delay(500);
    Serial.println();
}

bool testConnection(TwoWire &wire, int wireNumber) {
    wire.beginTransmission(I2C_SLAVE_ADDRESS_WII_ATTACHMENT);
    int error = wire.endTransmission();
    bool isConnected = error == 0;

    Serial.print("Wii attachment ");
    Serial.print(wireNumber, DEC);
    if (isConnected) {
        Serial.println(" connected :D");
    } else {
        Serial.print(" failed to connect with error: ");
        Serial.println(error, DEC);
    }

    return isConnected;
}
