#pragma once
#include "gca.h"

static constexpr gcStatus gcDefStatus = { 0x09, 0x00, 0x03 };
static constexpr gcReport gcDefReport = { 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x1F, 0x1F };
static constexpr gcOrigin gcDefOrigin = { 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x1F, 0x1F, 0x00, 0x00 };
static constexpr gcData gcDefault = { gcDefStatus, gcDefOrigin, gcDefReport };

class gcController : protected gcData {
public:
	gcController(const uint8_t p);
	void reset(void);
	bool begin(void);
	uint16_t getDevice(void);
	bool connected(void);
	bool read(void);
	bool getRumble(void);
	bool setRumble(bool rumble);
	bool end(void);
	gcStatus getStatus(void);
	gcOrigin getOrigin(void);
	gcReport getReport(void);
	gcData getData(void);

protected:
	const uint8_t pin;
	friend class gcConsole;
};

class gcConsole {
public:
	gcConsole(const uint8_t p);
	bool write(gcData &data);
	bool write(gcController &controller);
	bool write(gcReport &report);

protected:
	const uint8_t pin;
};

#include "gc.hpp"
