#include <Nintendo.h>
// #include <Wire.h>

CGamecubeConsole gc(2);
CGamecubeController gcc(3);

void setup() {
  Serial.begin(115200);
  while (!Serial) {}
}

void loop() {
  gcc.read();
  Gamecube_Report_t report = gcc.getReport();
  if (report.xAxis == 0x00) {
    report.xAxis = 0x80;
    report.yAxis = 0x80;
    report.cxAxis = 0x80;
    report.cyAxis = 0x80;
  }
  gc.write(report);
}
