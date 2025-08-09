#include <Arduino.h>
#include <Wire.h>

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
        
        // Teensy is little endian.
        uint8_t notZ                                : 1;
        uint8_t notC                                : 1;
        uint8_t accelerometerXAxisInsignificantBits : 2;
        uint8_t accelerometerYAxisInsignificantBits : 2;
        uint8_t accelerometerZAxisInsignificantBits : 2;
    };
};

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
bool isEncrypted = true;
bool isNunchuck = false;
void loop() {
    if (isEncrypted) {
        Wire.beginTransmission(I2C_SLAVE_ADDRESS_WII_ATTACHMENT);
        Wire.write(0xF0); // register addr
        Wire.write(0x55); // value
        isEncrypted = Wire.endTransmission() != 0;
        delayMicroseconds(I2C_TX_GRACE_TIME_MICROS);
        if (isEncrypted) return;

        Wire.beginTransmission(I2C_SLAVE_ADDRESS_WII_ATTACHMENT);
        Wire.write(0xFB);
        Wire.write(0x00);
        isEncrypted = Wire.endTransmission() != 0;
        delayMicroseconds(I2C_TX_GRACE_TIME_MICROS);
        return;
    }

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

        if (packet->accelerometerXAxisSignificantBits < 0x10) {
            Serial.println("O---------------");
        } else if (packet->accelerometerXAxisSignificantBits < 0x20) {
            Serial.println("-O--------------");
        } else if (packet->accelerometerXAxisSignificantBits < 0x30) {
            Serial.println("--O-------------");
        } else if (packet->accelerometerXAxisSignificantBits < 0x40) {
            Serial.println("---O------------");
        } else if (packet->accelerometerXAxisSignificantBits < 0x50) {
            Serial.println("----O-----------");
        } else if (packet->accelerometerXAxisSignificantBits < 0x60) {
            Serial.println("-----O----------");
        } else if (packet->accelerometerXAxisSignificantBits < 0x70) {
            Serial.println("------O---------");
        } else if (packet->accelerometerXAxisSignificantBits < 0x80) {
            Serial.println("-------O--------");
        } else if (packet->accelerometerXAxisSignificantBits < 0x90) {
            Serial.println("--------O-------");
        } else if (packet->accelerometerXAxisSignificantBits < 0xa0) {
            Serial.println("---------O------");
        } else if (packet->accelerometerXAxisSignificantBits < 0xb0) {
            Serial.println("----------O-----");
        } else if (packet->accelerometerXAxisSignificantBits < 0xc0) {
            Serial.println("-----------O----");
        } else if (packet->accelerometerXAxisSignificantBits < 0xd0) {
            Serial.println("------------O---");
        } else if (packet->accelerometerXAxisSignificantBits < 0xe0) {
            Serial.println("-------------O--");
        } else if (packet->accelerometerXAxisSignificantBits < 0xf0) {
            Serial.println("--------------O-");
        } else {
            Serial.println("---------------O");
        }

        // Serial.print(packet->accelerometerXAxisSignificantBits, HEX);
        // Serial.print(" ");
        // Serial.print(packet->accelerometerYAxisSignificantBits, HEX);
        // Serial.print(" ");
        // Serial.println(packet->accelerometerZAxisSignificantBits, HEX);
    }

    delayMicroseconds(I2C_TX_GRACE_TIME_MICROS);
}
