# SimpleSchedule (Arduino)
A Simple, timer-overflow-proof Scheduler. Suitable for pseudo-realtime applications or long delay intervals.

I wrote this as an experiment. It does **not** try to maintain _**accurate**_ delays between executions. Instead it ensures the right _**number**_ of executions were made at any time.

## Compatibility:
Should<sup>TM</sup> work with any Arduino-compatible boards.
Only tested on Atmega 328P.