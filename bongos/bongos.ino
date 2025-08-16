#include "Nunchuck.hpp"

// TODO Find c_cpp_properties.json in the git history and see if it's helpful.

Nunchuck nunchuck1(Wire);
Nunchuck nunchuck2(Wire1);

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    nunchuck1.setup();
    nunchuck2.setup();
}

void loop() {
    nunchuck1.loop();
    nunchuck2.loop();
    digitalWrite(LED_BUILTIN, LOW);

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
}
