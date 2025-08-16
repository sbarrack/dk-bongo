#include <Arduino.h>
#include <Wire.h>

#define E_I2C_TX_SUCCESS            0
#define E_I2C_TX_BUFFER_OVERFLOW    1
#define E_I2C_TX_REGISTER_NACK      2
#define E_I2C_TX_VALUE_NACK         3
#define E_I2C_TX_UNKNOWN            4

// TODO Find c_cpp_properties.json in the git history and see if it's helpful.

// https://wiibrew.org/wiki/Wiimote/Extension_Controllers
#define I2C_SLAVE_ADDRESS_WII_ATTACHMENT 0x52
#define I2C_TX_GRACE_TIME_MICROS 50
#define WII_ATTACHMENT_ID_SIZE 6

const uint8_t CONTROLLER_ID_NUNCHUCK[WII_ATTACHMENT_ID_SIZE] = {0x00, 0x00, 0xA4, 0x20, 0x00, 0x00};

union NunchuckPacket {
    uint8_t raw[6];

    struct {
        uint8_t joystickXAxis;
        uint8_t joystickYAxis;
        uint8_t accelerometerXAxisSignificantBits;
        uint8_t accelerometerYAxisSignificantBits;
        uint8_t accelerometerZAxisSignificantBits;
        
        // Teensy is little-endian.
        uint8_t notZ                                : 1;
        uint8_t notC                                : 1;
        uint8_t accelerometerXAxisInsignificantBits : 2;
        uint8_t accelerometerYAxisInsignificantBits : 2;
        uint8_t accelerometerZAxisInsignificantBits : 2;
    };
};

bool decrypt(TwoWire &i2c) {
    int errorCode;

    i2c.beginTransmission(I2C_SLAVE_ADDRESS_WII_ATTACHMENT);
    i2c.write(0xF0);   // Wii Remote register address
    i2c.write(0x55);   // Value
    errorCode = i2c.endTransmission();
    delayMicroseconds(I2C_TX_GRACE_TIME_MICROS);
    if (errorCode != E_I2C_TX_SUCCESS) {
        return false;
    }

    i2c.beginTransmission(I2C_SLAVE_ADDRESS_WII_ATTACHMENT);
    i2c.write(0xFB);
    i2c.write(0x00);
    errorCode = i2c.endTransmission();
    delayMicroseconds(I2C_TX_GRACE_TIME_MICROS);

    return errorCode == E_I2C_TX_SUCCESS;
}

void setup() {
    // Fast mode
    Wire.setClock(400000);
    Wire1.setClock(400000);

    Wire.begin();
    Wire1.begin();
    
    Serial.begin(115200);
    // make && sleep 1 && screen /dev/ttyACM0 115200
}

bool isEncrypted = true;
bool isNunchuck = false;
elapsedMicros loopTime; // TODO use this to make a timed queue for rx/tx
void loop() {
    loopTime = 0;
    isEncrypted = !decrypt(Wire);
    if (isEncrypted) return;

    if (!isNunchuck) {
        Wire.beginTransmission(I2C_SLAVE_ADDRESS_WII_ATTACHMENT);
        Wire.write(0xFA);
        Wire.write(0x00);
        if (Wire.endTransmission() != 0) return;
        delayMicroseconds(200);

        if (Wire.requestFrom(I2C_SLAVE_ADDRESS_WII_ATTACHMENT, WII_ATTACHMENT_ID_SIZE) != WII_ATTACHMENT_ID_SIZE)
            return;
        uint8_t attachmentId[WII_ATTACHMENT_ID_SIZE];
        isNunchuck = true;
        for (int i = 0; i < WII_ATTACHMENT_ID_SIZE; i++) {
            if (Wire.available() <= 0) {
                isNunchuck = false;
                return;
            }

            attachmentId[i] = Wire.read();
            
            if (attachmentId[i] != CONTROLLER_ID_NUNCHUCK[i]) {
                isNunchuck = false;
                return;
            }
        }
    } else {
        Wire.beginTransmission(I2C_SLAVE_ADDRESS_WII_ATTACHMENT);
        Wire.write(0x00); // fe 00 poll
        if (Wire.endTransmission() != 0) {
            return;
        }
        // FIXME Find how to detect when it's done rx/tx so I don't have to do this nonsense!
        delayMicroseconds(200);

        int size = Wire.requestFrom(I2C_SLAVE_ADDRESS_WII_ATTACHMENT, 6);
        if (size != 6) {
            return;
        }
        NunchuckPacket packet[6];
        for (int i = 0; i < 6; i++) {
            if (Wire.available() <= 0) {
                return;
            }

            packet->raw[i] = Wire.read();
        }

        Joystick.button(6, !packet->notC);
        Joystick.button(8, !packet->notZ);
        Joystick.Z(packet->joystickXAxis);
        Joystick.Zrotate(packet->joystickYAxis);
    }

    Serial.println(loopTime);
    delay(200);
}
