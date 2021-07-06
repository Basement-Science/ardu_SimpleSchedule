/*
A Simple, timer-overflow-proof Scheduler. Suitable for pseudo-realtime applications or long delay intervals.
*/
#include "SimpleSchedule.h"

/* Set up a scheduler instance. 
   interval_ms:      the interval you want between executions, in ms.
   executedFunction: Function pointer to whatever you want to run at this interval.
   errorFunction:    Function pointer to whatever you want to happen when intervals were skipped. */
SimpleSchedule::SimpleSchedule(const uint32_t interval_ms, void (*executedFunction)(), void (*errorFunction)(uint32_t) = &ErrorFkt) {
	interval = interval_ms;
	Task_funPtr = executedFunction;
	nextRun = lastRun + interval;

	StepsSkipped_funPtr = errorFunction;
}

/* Default error function. Set your own in the Constructor.
   Error functions get called if at least one intervall was skipped*/
void SimpleSchedule::ErrorFkt(uint32_t dontCare) {
	Serial.println("skip");
}

/* Checks if it's time to run your function. Should usually not be called externally. 
   Call Process(uint32_t time_ms) instead.*/
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

/* Poll this REGULARLY to ensure your function gets called*/
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