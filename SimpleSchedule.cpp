#include "SimpleSchedule.h"

SimpleSchedule::SimpleSchedule(const uint32_t interval_ms, void (*functionPointer)()) {
	interval = interval_ms;
	Task_funPtr = functionPointer;
	nextRun = lastRun + interval;

	StepsSkipped_funPtr = &ErrorFkt;
}

void SimpleSchedule::ErrorFkt(uint32_t dontCare) {
	Serial.println("skip");
}

bool SimpleSchedule::needsProcessing(uint32_t time_ms) {
	hasOverflowed = lastRun > time_ms;
	timeToRun = time_ms >= nextRun;
	if (timeToRun || hasOverflowed) {
		// check if we still need to keep waiting for an overflow
		waitForOverflow = waitForOverflow && !hasOverflowed;

		if (! waitForOverflow && (timeToRun || (waitForOverflow && hasOverflowed)) ) {
			// All good. Can be processed now.
			return true;
		}
	} 
	return false;
}

void SimpleSchedule::Process(uint32_t time_ms) {
	if (needsProcessing(time_ms)) {
		// Prepare for next run
		waitForOverflow = (nextRun > UINT32_MAX - interval) && !(hasOverflowed);

		nextRun += interval;	//	<<<--- V V V V V V V V V V V V V V V V V V V V
		lastRun = time_ms;		//	<<<--- distance between these is NOT exactly "interval"

		// did we skip processing within one or more intervals?
		if (nextRun <= time_ms && !waitForOverflow) {
			uint32_t executionsSkipped = time_ms - nextRun;
			//integer divide and ceil result:
			executionsSkipped = executionsSkipped / interval + (executionsSkipped % interval != 0);	
			nextRun += executionsSkipped * interval;

			// run the error function:
			(*StepsSkipped_funPtr)(executionsSkipped);
		}

		(*Task_funPtr)();	// run the task
	}
}