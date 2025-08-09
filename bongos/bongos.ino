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

struct NunchuckPacket {
    uint8_t joystickXAxis;
    uint8_t joystickYAxis;
    uint8_t accelerometerXAxisSignificantBits;
    uint8_t accelerometerYAxisSignificantBits;
    uint8_t accelerometerZAxisSignificantBits;
    
    uint8_t accelerometerZAxisInsignificantBits : 2;
    uint8_t accelerometerYAxisInsignificantBits : 2;
    uint8_t accelerometerXAxisInsignificantBits : 2;
    uint8_t c                                   : 1;
    uint8_t z                                   : 1;
};

bool isEncrypted = true;

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

int c = 0;
void loop() {
    if (isEncrypted) {
        Wire.beginTransmission(I2C_SLAVE_ADDRESS_WII_ATTACHMENT);
        Wire.write(0xF0);
        Wire.write(0x55);
        isEncrypted = Wire.endTransmission() != 0;
        delayMicroseconds(I2C_TX_GRACE_TIME_MICROS);
        if (isEncrypted) return;

        Wire.beginTransmission(I2C_SLAVE_ADDRESS_WII_ATTACHMENT);
        Wire.write(0xFB);
        Wire.write(0x00);
        isEncrypted = Wire.endTransmission() != 0;

        delay(200);
        return;
    }

    // ID
    Wire.beginTransmission(I2C_SLAVE_ADDRESS_WII_ATTACHMENT);
    Wire.write(0xFA);
    Wire.write(0x00);
    if (Wire.endTransmission() != 0) return;
    delayMicroseconds(I2C_TX_GRACE_TIME_MICROS);
    if (Wire.requestFrom(I2C_SLAVE_ADDRESS_WII_ATTACHMENT, WII_ATTACHMENT_ID_SIZE) != WII_ATTACHMENT_ID_SIZE) return;
    uint8_t attachmentId[WII_ATTACHMENT_ID_SIZE];
    bool isNunchuck = true;
    for (int i = 0; i < WII_ATTACHMENT_ID_SIZE; i++) {
        if (Wire.available() <= 0) {
            isNunchuck = false;
            break;
        }
        attachmentId[i] = Wire.read();
        if (attachmentId[i] != CONTROLLER_ID_NUNCHUCK[i]) {
            isNunchuck = false;
            break;
        }
    }
    Serial.print("isNunchuck? ");
    Serial.println(isNunchuck);

    // fe 00 poll
    
    delay(500);
    Serial.println();
    Serial.print(c++, DEC);
    Serial.println(" moms spaghetti");
}
