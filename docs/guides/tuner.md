# PID Tuning Guide

Vortex has two built-in tuner functions that make PID tuning faster. They print CSV output to the PROS terminal so you can graph error and power over time.

---

## Setup

Call the tuner from `autonomous()` — it takes over the chassis like a normal motion.

```cpp
void autonomous() {
    chassis.setPose(0, 0, 0);
    chassis.tuneLateral(24);   // drive forward 24 inches
    // OR
    chassis.tuneAngular(90);   // turn to 90 degrees
}
```

Open the PROS terminal, then run the auto program. Copy the printed lines starting with `TUNE_LAT:` or `TUNE_ANG:` into a spreadsheet and plot columns 2 (error) and 3 (power) against column 1 (time ms).

---

## Lateral Tuning Steps

Start with `kI=0` and `kD=0` in your `ControllerSettings`.

1. **Set kP only.** Increase it until the robot reaches the target but oscillates (rocks back and forth).
2. **Add kD.** Increase until the oscillations stop. You want the robot to approach the target in one smooth motion without bouncing.
3. **Add kI only if needed.** If the robot consistently stops 1–2 inches short of the target (steady-state error), add a small kI (try 0.001). Set `windupRange` to ~3× your `smallError`.
4. **Tune `slew`.** If the robot spins its wheels on launch, increase slew. 15–25 is typical.

**Signs of good lateral tuning:**
- Robot reaches target in ~1 second with no bounce
- It stops *cleanly* — doesn't crawl forward and lock
- Error on the graph reaches 0 and stays there

---

## Angular Tuning Steps

Same process. Use `chassis.tuneAngular(90)` to test a 90-degree turn.

1. **Set kP.** Increase until the robot swings to the heading but oscillates.
2. **Add kD.** Kills oscillation. Angular kD usually needs to be higher than lateral kD because inertia is high.
3. **kI is almost never needed for angular.** Skip it unless you have consistent heading offset.
4. **slew is usually 0 for turns** since you want full snap response.

**Signs of good angular tuning:**
- Robot reaches heading in ~0.5 seconds with no oscillation
- Final heading is within 1° of target

---

## Physical Calibration

Before tuning PID, get your `scrubFactor` and `trackWidth` right:

```cpp
void autonomous() {
    chassis.calibratePhysical();
}
```

The robot spins 10 full rotations. The output gives you:

```
Suggested trackWidth (2-wheel):  10.24
Suggested scrubFactor:           0.98
```

Update `Drivetrain` with these values, then re-run until the printed values stabilize. Do PID tuning *after* this step — incorrect physical constants make PID gains meaningless.
