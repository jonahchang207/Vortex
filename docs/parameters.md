# Tuning Parameters

This page explains every number you need to configure before your robot will move accurately.

---

## Physical Measurements

### `trackWidth`
The distance **in inches** between the contact patches of the left and right drive wheels — not the outside edge of the wheels, and not the robot frame. Measure from the center of each tire.

### `wheelDiameter`
The outer diameter of your drive wheels. Use the `vortex::Omniwheel` constants rather than measuring by hand — they account for real-world wear:

| Constant | Nominal | Actual |
|----------|---------|--------|
| `Omniwheel::NEW_275` | 2.75" | 2.75" |
| `Omniwheel::NEW_4` | 4" | 4.00" |
| `Omniwheel::OLD_4` | 4" | 4.18" |
| `Omniwheel::NEW_325` | 3.25" | 3.25" |

### `rpm`
Output shaft RPM **after all external gearing**. For a direct-drive blue cartridge: `600`. For a 36:60 green cartridge: `200 × (36/60) = 120`. For a common 36:60 blue: `600 × (36/60) = 360`.

### `horizontalDrift`
A dimensionless friction coefficient used to cap lateral speed on tight curves. Start with **2.0**. Higher = robot can slide more freely on curves; lower = tighter lateral cap. Most competition wheels are 1.5–3.5.

---

## Odometry Offsets

Each tracking wheel needs a signed offset from the robot's center of rotation:

### Vertical (parallel to drive) wheels
- **Positive** = left of center
- **Negative** = right of center

### Horizontal (perpendicular to drive) wheels
- **Positive** = forward of center
- **Negative** = behind center

To measure: place the robot on a flat surface and use calipers to measure from the robot's geometric center to the wheel contact point.

---

## scrubFactor

Accounts for wheel scrub (slip) during differential turns. Default is **1.0**.

**Run `chassis.calibratePhysical()` to get the correct value automatically.** It spins the robot 10 turns and computes how much the tracking wheel actually traveled vs how much the IMU reported, then prints the suggested values to the terminal.

A `scrubFactor` > 1 means the robot under-turns (increase it). < 1 means over-turns (decrease it).

---

## PID Constants

### kP (proportional)
The main driving force. Start by setting kI and kD to 0, then increase kP until the robot reaches its target but oscillates slightly.

### kD (derivative)
Dampens oscillation. Increase until oscillations stop. Too high and the robot becomes sluggish.

### kI (integral)
Only add if the robot consistently stops a few inches short of the target. Keep it very small — start at 0.001. Use `windupRange` to prevent integral windup.

### windupRange
The integral only accumulates when `|error|` is less than this. Prevents the integral from building up during large initial errors. Set to ~3× your `smallError` threshold.

### slew
Maximum motor power change per 10 ms loop iteration. Prevents wheel slip on fast starts. Typical value: **15–25** for lateral, **0** for angular (turns usually don't need it).

---

## Exit Conditions

Two independent exit conditions per axis — the motion exits when **either** triggers.

| Setting | Meaning |
|---------|---------|
| `smallError` | Error threshold (in or deg) to be considered "arrived" |
| `smallErrorTimeout` | Must stay within `smallError` for this many ms before exiting |
| `largeError` | Looser threshold — triggers a hard timeout if stuck here too long |
| `largeErrorTimeout` | Hard timeout from the first loop iteration (prevents getting stuck forever) |

Start with: `smallError=1`, `smallErrorTimeout=100`, `largeError=3`, `largeErrorTimeout=500`.
