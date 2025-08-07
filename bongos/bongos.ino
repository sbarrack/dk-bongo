#include <Arduino.h>
#include "gc.h"

// gcConsole gc(2);
gcController bongos(6);

gcReport gcr;

void setup() {
	// Linux is so cool! I can use the GNU multiplexer to view my Teensy's TTY :D
	// screen /dev/ttyACM0 115200
	Serial.begin(115200);
}

void loop() {
	gcr = gcDefault.report;

	bongos.read();
	gcr = bongos.getReport();

	Serial.print(gcr.raw[0], BIN);
	Serial.print(' ');
	Serial.print(gcr.raw[1], BIN);
	Serial.print(' ');
	Serial.print(gcr.raw[2], HEX);
	Serial.print(' ');
	Serial.print(gcr.raw[3], HEX);
	Serial.print(' ');
	Serial.print(gcr.raw[4], HEX);
	Serial.print(' ');
	Serial.print(gcr.raw[5], HEX);
	Serial.print(' ');
	Serial.print(gcr.raw[6], HEX);
	Serial.print(' ');
	Serial.println(gcr.raw[7], HEX);
	delay(100);

	// gc.write(gcr);
}
