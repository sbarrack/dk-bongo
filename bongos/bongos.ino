#include <Nintendo.h>
#include <Wire.h>

#define WII_EXT_ADDR 0x52
#define I2C_ADDR WII_EXT_ADDR

union {
  uint8_t raw[8];
  uint16_t raw16[4];
  uint32_t raw32[2];

  struct {
    uint8_t : 8;

    uint8_t dpad : 4;
    uint8_t : 4;
  };

  struct
  {
    uint8_t a : 1;
    uint8_t b : 1;
    uint8_t x : 1;
    uint8_t y : 1;
    uint8_t start : 1;
    uint8_t : 3;

    uint8_t dl : 1;
    uint8_t dr : 1;
    uint8_t dd : 1;
    uint8_t du : 1;
    uint8_t z : 1;
    uint8_t r : 1;
    uint8_t l : 1;
    uint8_t : 1;

    uint8_t sx;
    uint8_t sy;
    uint8_t cx;
    uint8_t cy;
    uint8_t lt;
    uint8_t rt;
  };
} rep;

enum WiiExtFull : uint64_t {
  Guitar = 0x030120a40000
};

union {
  uint8_t raw[6];
  WiiExtFull type;
} wiiExtID;

union {
  uint8_t raw[6];

  struct {
    uint8_t sx : 6;
    uint8_t : 1;
    uint8_t guitarType : 1;

    uint8_t sy : 6;
    uint8_t : 2;

    uint8_t fretbar : 5;  // funky values
    uint8_t : 3;

    uint8_t whammybar : 5;
    uint8_t : 3;

    uint8_t : 2;
    uint8_t start : 1;  // either/both +'s
    uint8_t : 1;
    uint8_t select : 1;  // - or starpower
    uint8_t : 1;
    uint8_t dd;  // strum down
    uint8_t : 1;

    uint8_t du;  // strum up
    uint8_t : 2;
    uint8_t yellow : 1;
    uint8_t green : 1;
    uint8_t blue : 1;
    uint8_t red : 1;
    uint8_t orange : 1;
  } guitar;
} wiiExtRep;

CGamecubeConsole gc(2);
// CGamecubeController gcc(3);

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  Wire.setClock(400000);  // fast mode
  Wire.begin(I2C_ADDR);   // slave
  // Wire.onReceive(get);
  // Wire.onRequest(send);

  // init
  Wire.beginTransmission(WII_EXT_ADDR);
  Wire.write(0xf0);  // @ 0x(4)a400f0
  Wire.write(0x55);  // write 0x55
  Wire.endTransmission();
  // while (Wire.endTransmission()) {}
  Serial.println("2");
  delayMicroseconds(38);  // min delay
  Wire.beginTransmission(WII_EXT_ADDR);
  Wire.write(0xfb);
  Wire.write(0);
  Wire.endTransmission();
  // while (Wire.endTransmission()) {}
  Serial.println("3");
  idWiiExt();
  Serial.println("5");
}

void loop() {
  Serial.println("step");
  // gcc.read();

  // poll wii ext
  delayMicroseconds(42);
  Wire.beginTransmission(WII_EXT_ADDR);
  Wire.write(0);  // @ addr 0x(4)a40000
                  // read
  while (Wire.endTransmission()) {}
  delayMicroseconds(149);
  int n = Wire.requestFrom(WII_EXT_ADDR, sizeof(wiiExtRep.raw));
  for (int i = 0; i < n; i++) {
    wiiExtRep.raw[i] = Wire.read();
  }

  rep.sx = 0x80;
  rep.sy = 0x80;
  rep.cx = 0x80;
  rep.cy = 0x80;
  rep.a = wiiExtRep.guitar.green;
  rep.b = wiiExtRep.guitar.red;
  rep.x = wiiExtRep.guitar.yellow;
  rep.y = wiiExtRep.guitar.blue;
  gc.write((CGamecubeController&)rep.raw);
}

// void get(int c) {
// }

// void send() {
//   Wire.write(rep.raw, sizeof(rep.raw));
// }

void idWiiExt() {
  delayMicroseconds(43);
  Wire.beginTransmission(WII_EXT_ADDR);
  Wire.write(0xfa);
  Wire.write(0);
  Wire.endTransmission();
  // while (Wire.endTransmission()) {}
  Serial.println("4");
  delayMicroseconds(38);
  int n = Wire.requestFrom(WII_EXT_ADDR, sizeof(wiiExtID.raw));
  for (int i = 0; i < n; i++) {
    wiiExtID.raw[i] = Wire.read();
    Serial.write(wiiExtID.raw[i]);
  }
}