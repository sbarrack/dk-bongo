#pragma once
#include <Arduino.h>
#include "gcb.h"

union gcReport {
	uint8_t raw[8];
	//gcc
	struct {
		uint8_t errstat : 1;
		uint8_t errlatch : 1;
		uint8_t origin : 1;
		uint8_t start : 1;
		uint8_t y : 1;
		uint8_t x : 1;
		uint8_t b : 1;
		uint8_t a : 1;

		uint8_t useOrigin : 1; // always high
		uint8_t l : 1;
		uint8_t r : 1;
		uint8_t z : 1;
		uint8_t dup : 1;
		uint8_t ddown : 1;
		uint8_t dright : 1;
		uint8_t dleft : 1;

		uint8_t xAxis;
		uint8_t yAxis;
		uint8_t cxAxis;
		uint8_t cyAxis;
		uint8_t left;
		uint8_t right;
	};
	//dkb
	struct {
		uint8_t errstat0 : 1;
		uint8_t errlatch0 : 1;
		uint8_t origin0 : 1;
		uint8_t start0 : 1;
		uint8_t tl : 1;
		uint8_t tr : 1;
		uint8_t bl : 1;
		uint8_t br : 1;

		uint8_t useOrigin0 : 1;
		uint8_t zero0 : 7;

		uint8_t zero[5];
		uint8_t mic;
	};
};

union gcOrigin {
	uint8_t raw[10];

	struct {
		gcReport inititalData;
		uint8_t deadzone0;
		uint8_t deadzone1;
	};
};

struct gcData {
	gcStatus status;
	gcOrigin origin;
	gcReport report;
};

#ifdef __cplusplus
extern "C" {
#endif
	bool gcInit(const uint8_t pin, gcStatus* status);
	bool gcReset(const uint8_t pin, gcStatus* status);
	bool gcCenter(const uint8_t pin, gcOrigin* origin);
	bool gcRead(const uint8_t pin, gcReport* report, const bool rumble);
	uint8_t gcWrite(const uint8_t pin, gcStatus* status, gcOrigin* origin, gcReport* report);
#ifdef __cplusplus
}
#endif
