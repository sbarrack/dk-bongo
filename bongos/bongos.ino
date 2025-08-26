#include "Nunchuck.hpp"

// TODO Find c_cpp_properties.json in the git history and see if it's helpful.

Nunchuck nunchuck1(Wire);
// TODO Fix wiring for Wire1. (It's not my solder joint.)

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    nunchuck1.setup();
}

void loop() {
    nunchuck1.loop();

    // TODO Input from Bongos.
    // FIXME Output to GC.
}
