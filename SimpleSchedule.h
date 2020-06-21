#ifndef _SIMPLESCHEDULE_h
#define _SIMPLESCHEDULE_h

#include <inttypes.h>
#include <Arduino.h>

class SimpleSchedule {
private:
	static void ErrorFkt(uint32_t dontCare);

	uint32_t interval;
	void(*Task_funPtr)();
	
	uint32_t lastRun = 0;
	uint32_t nextRun;

	bool waitForOverflow = false;
	bool hasOverflowed = false;
	bool timeToRun = false;
public:
	SimpleSchedule(const uint32_t interval_ms, void(*executedFunction)());

	bool needsProcessing(uint32_t time_ms);
	void Process(uint32_t time_ms);

	void(*StepsSkipped_funPtr)(uint32_t steps);
};


#endif