# PID, ExitCondition & Timer

---

## PID

`#include "Vortex/pid.hpp"`

Discrete PID controller. Called once per 10 ms loop.

```cpp
vortex::PID pid(kP, kI, kD, windupRange, signFlipReset);
pid.reset();                   // clear integral and previous error
double output = pid.update(error); // feed error, get power output
```

- **windupRange**: integral only accumulates when `|error| < windupRange`. Set to 0 to disable windowing.
- **signFlipReset**: when `true`, resets the integral when error crosses zero. Prevents integral windup on approach.

---

## ExitCondition

`#include "Vortex/exitcondition.hpp"`

Determines when a motion is "done."

```cpp
vortex::ExitCondition exit(
    1.0,   // error range (in or deg) to be considered "at target"
    100,   // settle time (ms): robot must stay in range for this long
    500    // maxTime (ms): hard timeout from first update() call
);

exit.reset();
exit.update(error);
if (exit.getExit()) { /* done */ }
```

Both conditions are independent — the motion exits when **either** triggers:
1. Error stays within `range` for `settleTime` ms
2. `maxTime` ms has passed since first update

---

## Timer

`#include "Vortex/timer.hpp"`

Countdown timer with pause/resume. All getters are **idempotent** — calling them multiple times in one loop does not accumulate extra time.

```cpp
vortex::Timer t(1000);  // 1-second countdown

t.isDone()          // true when elapsed >= period
t.getTimeLeft()     // ms remaining
t.getTimePassed()   // ms elapsed
t.pause()           // freeze elapsed time
t.resume()          // continue from where paused
t.reset()           // restart countdown
t.waitUntilDone()   // blocking wait (5 ms poll)
```
