#include <Arduino.h>
#include <Wire.h>

#define E_I2C_TX_SUCCESS            0
#define E_I2C_TX_BUFFER_OVERFLOW    1
#define E_I2C_TX_REGISTER_NACK      2
#define E_I2C_TX_VALUE_NACK         3
#define E_I2C_TX_UNKNOWN            4

// https://wiibrew.org/wiki/Wiimote/Extension_Controllers
#define I2C_SLAVE_ADDRESS_WII_ATTACHMENT 0x52
/**
 * This worked as low as 34us on USB power, but it might need more grace on
 * lower CPU speeds which varies with Vin.
 */
#define I2C_TX_GRACE_TIME_MICROS 200
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

/**
 * @param _wireNumber For debugging
 */
bool decrypt(TwoWire &i2c, int _wireNumber = 0) {
    int errorCode;

    i2c.beginTransmission(I2C_SLAVE_ADDRESS_WII_ATTACHMENT);
    i2c.write(0xF0);   // Wii Remote register address
    i2c.write(0x55);   // Value
    errorCode = i2c.endTransmission();
    delayMicroseconds(I2C_TX_GRACE_TIME_MICROS);
    if (errorCode != E_I2C_TX_SUCCESS) {
        // Serial.print("Decrypting Wire");
        // Serial.print(_wireNumber, DEC);
        // Serial.print(" part 1/2 failed with code: ");
        // Serial.println(errorCode);
        return false;
    }

    i2c.beginTransmission(I2C_SLAVE_ADDRESS_WII_ATTACHMENT);
    i2c.write(0xFB);
    i2c.write(0x00);
    errorCode = i2c.endTransmission();
    delayMicroseconds(I2C_TX_GRACE_TIME_MICROS);
    // if (errorCode != E_I2C_TX_SUCCESS) {
    //     Serial.print("Decrypting Wire");
    //     Serial.print(_wireNumber, DEC);
    //     Serial.print(" part 2/2 failed with code: ");
    //     Serial.println(errorCode);
    // }

    return errorCode == E_I2C_TX_SUCCESS;
}

void setup() {
    // Fast mode
    Wire.setClock(400000);
    // Wire1.setClock(400000);

    Wire.begin();
    // Wire1.begin();
    
    Serial.begin(115200);
    while (!Serial);
    delay(2000);
    // make && sleep 1 && screen /dev/ttyACM0 115200
}

// FIXME handle rx fail
// FIXME handle tx error based on code
// FIXME multiple controllers
bool isEncrypted = true;
bool isNunchuck = false;
void loop() {
    isEncrypted = !decrypt(Wire);
    if (isEncrypted) return;

    if (!isNunchuck) {
        Wire.beginTransmission(I2C_SLAVE_ADDRESS_WII_ATTACHMENT);
        Wire.write(0xFA);
        Wire.write(0x00);
        if (Wire.endTransmission() != 0) return;
        delayMicroseconds(I2C_TX_GRACE_TIME_MICROS);

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
            Serial.println("poll failed");
            return;
        }
        // FIXME Find how to detect when it's done rx/tx so I don't have to do this nonsense!
        delayMicroseconds(I2C_TX_GRACE_TIME_MICROS);

        int size = Wire.requestFrom(I2C_SLAVE_ADDRESS_WII_ATTACHMENT, 6);
        if (size != 6) {
            Serial.print("wrong size rx: ");
            Serial.println(size, DEC);
            return;
        }
        NunchuckPacket packet[6];
        for (int i = 0; i < 6; i++) {
            if (Wire.available() <= 0) {
                Serial.println("missing bytes in packet");
                return;
            }

            packet->raw[i] = Wire.read();
        }

        if (packet->accelerometerZAxisSignificantBits < 0x10) {
            Serial.println("O---------------");
        } else if (packet->accelerometerZAxisSignificantBits < 0x20) {
            Serial.println("-O--------------");
        } else if (packet->accelerometerZAxisSignificantBits < 0x30) {
            Serial.println("--O-------------");
        } else if (packet->accelerometerZAxisSignificantBits < 0x40) {
            Serial.println("---O------------");
        } else if (packet->accelerometerZAxisSignificantBits < 0x50) {
            Serial.println("----O-----------");
        } else if (packet->accelerometerZAxisSignificantBits < 0x60) {
            Serial.println("-----O----------");
        } else if (packet->accelerometerZAxisSignificantBits < 0x70) {
            Serial.println("------O---------");
        } else if (packet->accelerometerZAxisSignificantBits < 0x80) {
            Serial.println("-------O--------");
        } else if (packet->accelerometerZAxisSignificantBits < 0x90) {
            Serial.println("--------O-------");
        } else if (packet->accelerometerZAxisSignificantBits < 0xa0) {
            Serial.println("---------O------");
        } else if (packet->accelerometerZAxisSignificantBits < 0xb0) {
            Serial.println("----------O-----");
        } else if (packet->accelerometerZAxisSignificantBits < 0xc0) {
            Serial.println("-----------O----");
        } else if (packet->accelerometerZAxisSignificantBits < 0xd0) {
            Serial.println("------------O---");
        } else if (packet->accelerometerZAxisSignificantBits < 0xe0) {
            Serial.println("-------------O--");
        } else if (packet->accelerometerZAxisSignificantBits < 0xf0) {
            Serial.println("--------------O-");
        } else {
            Serial.println("---------------O");
        }
    }

    delayMicroseconds(I2C_TX_GRACE_TIME_MICROS);
}
