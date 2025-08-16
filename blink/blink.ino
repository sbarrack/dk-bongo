#include <Arduino.h>
#include <Wire.h>

// aka this sketch is my code dump (doesn't actually blink lol but same idea, place to test crap)

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
// Maybe try writing the the 00 locations including the data format. Also maybe
// ...try decoding that sequence. Also also, dump guitar once I fix mine.
// Actually also, try writing more than two bytes.
// Last byte (writeable) = data format
// 00 = nunchuck
// 01 = classic controller
// 02 = classic controller?
// 03 = classic controller?/guitar
// // Code to dump a wiimote extension
// Wire.beginTransmission(0x52);
// Wire.write(0xF0);
// Wire.write(0x55);
// Wire.endTransmission();
// delayMicroseconds(50);
// uint8_t i = 0;
// do {
//     Wire.beginTransmission(0x52);
//     Wire.write(i);
//     Wire.endTransmission();
//     i += 64;
//     delayMicroseconds(3200);
//     Wire.requestFrom(0x52, 64);
//     int c = 1;
//     while (Wire.available() > 0) {
//         uint8_t byte = Wire.read();
//         if (byte < 0x10) Serial.print(0);
//         Serial.print(byte, HEX);
//         if (c++ >= 16) {
//             c = 1;
//             Serial.println();
//         } else {
//             Serial.print(' ');
//         }
//     }
// } while (i != 0);

void setup() {
    Wire.setClock(1000000);
    Wire.begin();
    Serial.begin(115200);
    while(!Serial);
    // make && sleep 1 && screen /dev/ttyACM0 115200

    Wire.beginTransmission(0x52);
    Wire.write(0xF0);
    Wire.write(0x55);
    Wire.endTransmission();
    delayMicroseconds(50);
    Wire.beginTransmission(0x52);
    Wire.write(0);
    Wire.endTransmission();
    delayMicroseconds(300);
    Serial.println(Wire.requestFrom(0x52, 72));
    while (Wire.available() > 0) {
        Serial.print(' ');
        Serial.print(Wire.read(), HEX);
    }
}

void loop() {
}
