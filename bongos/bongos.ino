// Board: Arduino Nano
// Processor: ATmega328P (Old Bootloader)
#include <Wire.h>      // I2C/TWI
#include <Nintendo.h>  // Nicohood

// TODO Fix wiring so that the Arduino doesn't need to be plugged in via USB to work.

#define I2C_ADDR 0x52
struct NunchuckReport {
  uint8_t sx;

  uint8_t sy;

  uint32_t ax : 10;
  uint32_t ay : 10;
  uint32_t az : 10;
  uint32_t c : 1;
  uint32_t z : 1;
};
const uint8_t NUNCHUCK_ID[6] = { 0x00, 0x00, 0xA4, 0x20, 0x00, 0x00 };
uint8_t attachmentId[6];
bool isNunchuck;

CGamecubeConsole gc(2);
CGamecubeController gcc(3);

void setup() {
  Serial.begin(115200);

  // Init attachment unencrypted
  Wire.setTimeout(10);
  Wire.setClock(400000);
  Wire.begin();
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0xF0);
  Wire.write(0x55);
  // while (Wire.endTransmission()) {}
  Wire.endTransmission();
  delay(1);
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0xFB);
  Wire.write(0);
  // while (Wire.endTransmission()) {}
  Wire.endTransmission();
  delay(1);

  // ID the attachment
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0xFA);
  Wire.write(0);
  // while (Wire.endTransmission()) {}
  Wire.endTransmission();
  // delayMicroseconds(36);
  // delay(1);
  Wire.requestFrom(I2C_ADDR, 6);
  Wire.readBytes(attachmentId, 6);

  isNunchuck = true;
  for (int i = 0; i < 6; i++) {
    if (attachmentId[i] != NUNCHUCK_ID[i]) {
      isNunchuck = false;
      break;
    }
  }
  Serial.write(isNunchuck ? "SUCCESS: Nunchuck connected" : "FAIL: Nunchuck not detected");
}

void loop() {
  // Wire.beginTransmission(I2C_ADDR);
  // Wire.write(0);
  // // while (Wire.endTransmission()) {}
  // Wire.endTransmission();
  // // delayMicroseconds(157);
  // // delay(1);
  // uint8_t attachmentRaw[6];
  // Wire.requestFrom(I2C_ADDR, 6);
  // Wire.readBytes(attachmentRaw, 6);

  // NunchuckReport nunchuck_input;
  // nunchuck_input.sx = attachmentRaw[0];
  // nunchuck_input.sy = attachmentRaw[1];

  // nunchuck_input.ax = attachmentRaw[2] << 2 | (attachmentRaw[5] >> 2 & 3);
  // nunchuck_input.ay = attachmentRaw[3] << 2 | (attachmentRaw[5] >> 4 & 3);
  // nunchuck_input.az = attachmentRaw[4] << 2 | attachmentRaw[5] >> 6;
  // nunchuck_input.c = ~attachmentRaw[5] >> 1;
  // nunchuck_input.z = ~attachmentRaw[5];


  gcc.read();
  Gamecube_Report_t input = gcc.getReport();
  Gamecube_Report_t output = input;
  bool isBongos = input.xAxis == 0x00;
  if (isBongos) {
    output.xAxis = 0x80;
    output.yAxis = 0x80;
    // if (isNunchuck) {
    //   output.xAxis = nunchuck_input.sx;
    //   output.yAxis = nunchuck_input.sy;
    //   if (nunchuck_input.z == 0x1) {
    //     output.left = 0x80;  // Analog
    //   }
    //   if (nunchuck_input.c == 0x1) {
    //     output.l = 0x1;  // Digital
    //   }
    // }
    output.cxAxis = 0x80;
    output.cyAxis = 0x80;
  }
  gc.write(output);
}
