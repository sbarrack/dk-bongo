#include <Arduino.h>
#include <Wire.h>

// Complete Nunchuck memory dump :)
//    & | 0x0 0x1 0x2 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xa 0xb 0xc 0xd 0xe 0xf
// 0x00 | 85  82  50  6E  92  27  00  00  00  00  00  00  00  00  00  00 <-(&0x00-0x05 = controller inputs)
// 0x10 | 00  00  00  00  00  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  7B
// 0x20 | 7A  7A  36  AF  AC  AF  18  DF  19  82  E3  25  81  1F  74  7B
// 0x30 | 7A  7A  36  AF  AC  AF  18  DF  19  82  E3  25  81  1F  74  FF
// 0x40 | FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF
// 0x50 | FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF
// 0x60 | FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF
// 0x70 | FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF
// 0x80 | FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF
// 0x90 | FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF
// 0xa0 | FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF
// 0xb0 | FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF
// 0xc0 | FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF
// 0xd0 | FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF
// 0xe0 | FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF  FF
// 0xf0 | 55  00  00  00  00  00  00  00  00  00  00  00  A4  20  00  00 <-(&0xfe = data format)
//        ^^(0x55 = the decryption key)           ^------(ID)----------^
// All of the 0xFF bytes = nothing; they're just unwritable empty space.
// The 0x00 bytes after the controller data (i.e. addr &0x06-0x14) might actually be for more controller data if there were any.
// Idk what the weird repeating sequence from &0x1f-0x3e is. Weird that it's not perfectly aligned.
// 72 bytes is the max it lets me read in one request.
// TODO try writing data format
// LATER once i fix the frayed guitar cable, also dump that

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

// Last byte (writeable) = data format
// 00 = nunchuck
// 01 = classic controller
// 02 = classic controller?
// 03 = classic controller?/guitar
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
    // if (errorCode != E_I2C_TX_SUCCESS) {
    //     return false;
    // }

    // I figured this out. Reading &0xfb doesn't actually do anything. It's just a way to check
    // if your decryption worked because it's the only byte where all (known) attachment IDs have 0x00 when
    // decrypted. So I really should be reading 1 byte if I even gave a darn. (If it failed to decrypt, all
    // of the bytes in the ID would be 0xff.
    //
    // i2c.beginTransmission(I2C_SLAVE_ADDRESS_WII_ATTACHMENT);
    // i2c.write(0xFB);
    // // i2c.write(0x00);
    // errorCode = i2c.endTransmission();
    // delayMicroseconds(I2C_TX_GRACE_TIME_MICROS);

    return errorCode == E_I2C_TX_SUCCESS;
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    
    Serial.begin(115200);
    while(!Serial);
    // make && sleep 1 && screen /dev/ttyACM0 115200

    // Fast mode
    Wire.setClock(400000);
    // Wire1.setClock(400000);

    Wire.begin();
    // Wire1.begin();

    while (!decrypt(Wire));
    uint8_t i = 0;
    do {
        Wire.beginTransmission(0x52);
        Wire.write(i);
        Wire.endTransmission();
        i += 64;
        delayMicroseconds(3200);
        Wire.requestFrom(0x52, 64);
        int c = 1;
        while (Wire.available() > 0) {
            uint8_t byte = Wire.read();
            if (byte < 0x10) Serial.print(0);
            Serial.print(byte, HEX);
            if (c++ >= 16) {
                c = 1;
                Serial.println();
            } else {
                Serial.print(' ');
            }
        }
    } while (i != 0);
    digitalWrite(LED_BUILTIN, LOW);
}

// bool isEncrypted = true;
// bool isNunchuck = false;
// elapsedMicros loopTime; // TODO use this to make a timed queue for rx/tx
void loop() {
    // // loopTime = 0;
    // isEncrypted = !decrypt(Wire);
    // if (isEncrypted) return;

    // if (!isNunchuck) {
    //     Wire.beginTransmission(I2C_SLAVE_ADDRESS_WII_ATTACHMENT);
    //     Wire.write(0xFA);
    //     if (Wire.endTransmission() != 0) return;
    //     delayMicroseconds(200);

    //     if (Wire.requestFrom(I2C_SLAVE_ADDRESS_WII_ATTACHMENT, WII_ATTACHMENT_ID_SIZE) != WII_ATTACHMENT_ID_SIZE)
    //         return;
    //     uint8_t attachmentId[WII_ATTACHMENT_ID_SIZE];
    //     isNunchuck = true;
    //     for (int i = 0; i < WII_ATTACHMENT_ID_SIZE; i++) {
    //         if (Wire.available() <= 0) {
    //             isNunchuck = false;
    //             return;
    //         }

    //         attachmentId[i] = Wire.read();
            
    //         if (attachmentId[i] != CONTROLLER_ID_NUNCHUCK[i]) {
    //             isNunchuck = false;
    //             return;
    //         }
    //     }
    //     digitalWrite(LED_BUILTIN, LOW);
    // } else {
    //     Wire.beginTransmission(I2C_SLAVE_ADDRESS_WII_ATTACHMENT);
    //     Wire.write(0x00); // 00 poll
    //     if (Wire.endTransmission() != 0) {
    //         return;
    //     }
    //     // FIXME Find how to detect when it's done rx/tx so I don't have to do this nonsense!
    //     delayMicroseconds(200);

    //     int size = Wire.requestFrom(I2C_SLAVE_ADDRESS_WII_ATTACHMENT, 6);
    //     if (size != 6) {
    //         return;
    //     }
    //     NunchuckPacket packet[6];
    //     for (int i = 0; i < 6; i++) {
    //         if (Wire.available() <= 0) {
    //             return;
    //         }

    //         packet->raw[i] = Wire.read();
    //     }

    //     Joystick.button(6, !packet->notC);
    //     Joystick.button(8, !packet->notZ);
    //     Joystick.Z(packet->joystickXAxis);
    //     Joystick.Zrotate(packet->joystickYAxis);
    // }

    // Serial.println(loopTime);
}
