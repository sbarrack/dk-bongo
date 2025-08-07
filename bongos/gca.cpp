#include "gca.h"

bool gcInit(const uint8_t pin, gcStatus* status) {
	uint8_t command[] = { 0x00 };
	uint8_t receivedBytes = gcSendGet(pin, command, sizeof(command), (uint8_t*)status, sizeof(gcStatus));
	return (receivedBytes == sizeof(gcStatus));
}

bool gcReset(const uint8_t pin, gcStatus* status) {
	uint8_t command[] = { 0xFF };
	uint8_t receivedBytes = gcSendGet(pin, command, sizeof(command), (uint8_t*)status, sizeof(gcStatus));
	return (receivedBytes == sizeof(gcStatus));
}

bool gcCenter(const uint8_t pin, gcOrigin* origin) {
	uint8_t command[] = { 0x41 };
	uint8_t receivedBytes = gcSendGet(pin, command, sizeof(command), (uint8_t*)origin, sizeof(gcStatus));
	return (receivedBytes == sizeof(gcOrigin));
}

bool gcRead(const uint8_t pin, gcReport* report, const bool rumble) {
	uint8_t command[] = { 0x40, 0x03, rumble };
	uint8_t receivedBytes = gcSendGet(pin, command, sizeof(command), (uint8_t*)report, sizeof(gcReport));
	return (receivedBytes == sizeof(gcReport));
}

uint8_t gcWrite(const uint8_t pin, gcStatus* status, gcOrigin* origin, gcReport* report) {
	uint8_t ret = 0;

	uint8_t bitMask = digitalPinToBitMask(pin);
	uint8_t port = digitalPinToPort(pin);

	volatile uint8_t* modePort = portModeRegister(port);
	volatile uint8_t* outPort = portOutputRegister(port);
	volatile uint8_t* inPort = portInputRegister(port);

	uint8_t oldSREG = SREG;
	cli();

	uint8_t command[3] = { 0, 0, 0 };
	uint8_t receivedBytes = gcGet(command, sizeof(command), modePort, outPort, inPort, bitMask);

	// Init
	if (receivedBytes == 1 && command[0] == 0x00) {
		gcSend(status->raw, sizeof(gcStatus), modePort, outPort, bitMask);
		ret = 1;
	}
	// Get origin
	else if (receivedBytes == 1 && command[0] == 0x41) {
		gcSend(origin->raw, sizeof(gcOrigin), modePort, outPort, bitMask);
		ret = 2;
	}
	// Poll
	else if (receivedBytes == 3 && command[0] == 0x40 && command[1] == 0x03) {
		gcSend(report->raw, sizeof(gcReport), modePort, outPort, bitMask);
		ret = 3;

		if ((command[2] % 4) == 0x01)
			ret = 4;
		else if ((command[2] % 4) == 0x02)
			ret = 5;
		else if ((command[2] % 4) == 0x03)
			ret = 6;
	}

	SREG = oldSREG;

	return ret;
}
