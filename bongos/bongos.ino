#include <Wire.h>  // I2C/TWI

#include <Nintendo.h>  // Nicohood

CGamecubeConsole gc(2);
CGamecubeController gcc(3);

void setup() {
}

void loop() {
  gcc.read();
  Gamecube_Report_t input = gcc.getReport();
  Gamecube_Report_t output = input;
  bool isBongos = input.xAxis == 0x00;
  if (isBongos) {
    output.xAxis = 0x80;
    output.yAxis = 0x80;
    output.cxAxis = 0x80;
    output.cyAxis = 0x80;
  }
  gc.write(output);
}
