#include "Nunchuck.hpp"

// TODO Find c_cpp_properties.json in the git history and see if it's helpful.

Nunchuck nunchuck1(Wire);
Nunchuck nunchuck2(Wire1);

void setup() {
    Serial.begin(115200);
    while(!Serial);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    nunchuck1.setup();
    nunchuck2.setup();
}

void loop() {
    nunchuck1.loop();
    nunchuck2.loop();

    Serial.print(nunchuck1.getC() ? "C" : "_");
    Serial.print(nunchuck1.getZ() ? "Z" : "_");
    Serial.print(" ");
    Serial.print(nunchuck1.getX(), HEX);
    Serial.print(",");
    Serial.print(nunchuck1.getY(), HEX);
    Serial.print(" | ");
    Serial.print(nunchuck2.getC() ? "C" : "_");
    Serial.print(nunchuck2.getZ() ? "Z" : "_");
    Serial.print(" ");
    Serial.print(nunchuck2.getX(), HEX);
    Serial.print(",");
    Serial.println(nunchuck2.getY(), HEX);
    delay(10);
}
