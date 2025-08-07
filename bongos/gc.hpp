#pragma once

gcController::gcController(const uint8_t p) : pin(p) {}

void gcController::reset(void) {
	status = gcStatus();
}

bool gcController::begin(void) {
	if (!gcInit(pin, &status)) {
		reset();
		return false;
	}

	if (!gcCenter(pin, &origin))
		return false;

	return true;
}

uint16_t gcController::getDevice(void) {
	// (Optional) swap the first two bytes to compare it easy with the documentation
	return status.device;
}

bool gcController::connected(void) {
	return status.device != 0;
}

bool gcController::read(void) {
	if (!connected() && !begin())
		return false;

	if (!gcRead(pin, &report, status.rumble)) {
		reset();
		return false;
	}

	if (report.origin) {
		reset();
		return false;
	}

	return true;
}

bool gcController::getRumble(void) {
	return status.rumble;
}

bool gcController::setRumble(bool rumble) {
	bool oldRumble = getRumble();
	status.rumble = rumble;
	return oldRumble;
}

bool gcController::end(void) {
	if (connected() || getRumble()) {
		gcReport tmp;
		return gcRead(pin, &tmp, false);
	}

	return false;
}

gcStatus gcController::getStatus(void) {
	return status;
}

gcOrigin gcController::getOrigin(void) {
	return origin;
}

gcReport gcController::getReport(void) {
	return report;
}

gcData gcController::getData(void) {
	gcData* dataPtr = this;
	gcData data;
	memcpy(&data, dataPtr, sizeof(data));
	return data;
}

gcConsole::gcConsole(const uint8_t p) : pin(p) {}

bool gcConsole::write(gcData &data) {
	if (data.report.origin)
		return false;

	uint8_t oldSREG = SREG;
	cli();

	uint8_t ret = gcWrite(pin, &data.status, &data.origin, &data.report);

	if (ret == 1)
		ret = gcWrite(pin, &data.status, &data.origin, &data.report);
	if (ret == 2)
		ret = gcWrite(pin, &data.status, &data.origin, &data.report);

	SREG = oldSREG;

	if (ret == 3) {
		data.status.rumble = false;
		return true;
	}
	else if (ret == 4) {
		data.status.rumble = true;
		return true;
	}
	else if (ret == 5) {
		data.status.rumble = false;
		return true;
	}

	return false;
}

bool gcConsole::write(gcController &controller) {
	return write(controller);
}

bool gcConsole::write(gcReport &report) {
	gcData data = gcDefault;

	memcpy(&data.report, &report, sizeof(data.report));
	return write(data);
}