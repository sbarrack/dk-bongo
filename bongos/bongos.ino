#ifdef __MK20DX256__
#include "Nunchuck.hpp"

#define CONSOLE_PIN     2
#define CONTROLLER_PIN  14

// TODO Find c_cpp_properties.json in the git history and see if it's helpful.

Nunchuck nunchuck1(Wire);
// TODO Fix wiring for Wire1. (It's not my solder joint.)

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    nunchuck1.setup();
    
    Serial.begin(115200);
    while(!Serial);
    // make && sleep 1 && screen /dev/ttyACM0 115200

    // // Console
    // pinMode(CONSOLE_PIN, INPUT);
    // attachInterrupt(CONSOLE_PIN, foobar, LOW);
    // NVIC_SET_PRIORITY(IRQ_PORTD, 0);
    // Controller
    pinMode(CONTROLLER_PIN, OUTPUT);
    // attachInterrupt(CONTROLLER_PIN, relayToConsole, FALLING);

    // 32-bit cpu cycle counter
    // ARM_DWT_CYCCNT

    // delayNanoseconds() is a thing
}

int i = 0;
void foobar() {
    cli();  // Disable interrupts globally
    Serial.println(i++);
    sei();  // Enable interrupts globally
}

void loop() {
    nunchuck1.loop();

    digitalWrite(LED_BUILTIN, nunchuck1.getZ());
    delay(100);
}

#endif
