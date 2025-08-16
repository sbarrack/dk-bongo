#include "Nunchuck.hpp"

// TODO Find c_cpp_properties.json in the git history and see if it's helpful.

Nunchuck nunchuck1(Wire);

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    Joystick.useManualSend(true);

    nunchuck1.setup();
}

void loop() {
    nunchuck1.loop();

    digitalWrite(LED_BUILTIN, !nunchuck1.getZ());
    Joystick.button(1, nunchuck1.getC());
    Joystick.button(2, nunchuck1.getZ());
    Joystick.X(nunchuck1.getX());
    Joystick.Y(nunchuck1.getY());
    Joystick.Z(nunchuck1.getAz());
    Joystick.Zrotate(nunchuck1.getAx());
    Joystick.sliderLeft(nunchuck1.getAy());

    Joystick.send_now();
}
