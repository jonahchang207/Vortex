# API Overview

All Vortex classes and functions live in the `vortex::` namespace. Include `"Vortex/api.hpp"` to get everything.

---

## Core Classes

| Class | Description |
|-------|-------------|
| [`Chassis`](api/chassis.md) | Main robot controller — odometry, PID, and all motions |
| [`Odom`](api/odom.md) | Arc odometry engine (managed by Chassis) |
| [`TrackingWheel`](api/tracking_wheels.md) | Encoder abstraction (Rotation, ADI, or Motor) |
| [`PID`](api/pid.md) | Discrete PID controller |
| [`ExitCondition`](api/pid.md) | Settle + timeout exit logic |
| [`Timer`](api/pid.md) | Countdown timer with pause/resume |
| [`AutonSelector`](api/auton_selector.md) | LLEMU-based routine picker |

---

## Configuration Structs

| Struct | Description |
|--------|-------------|
| `Drivetrain` | Motor groups, track width, RPM, drift, scrub |
| `ControllerSettings` | PID gains + exit condition thresholds |
| `OdomSensors` | Bundle of up to 4 tracking wheels + IMU |
| `MoveToPoseParams` | Per-call overrides for `moveToPose` |
| `MoveToPointParams` | Per-call overrides for `moveToPoint` |
| `TurnToHeadingParams` | Per-call overrides for `turnToHeading` |
| `TurnToPointParams` | Per-call overrides for `turnToPoint` |
| `SwingToHeadingParams` | Per-call overrides for `swingToHeading` |

---

## Utility

| Function | Description |
|----------|-------------|
| [`angleError`](api/util.md) | Signed angular error with direction preference |
| [`getCurvature`](api/util.md) | Signed curvature between a pose and a target |
| [`motionProfile`](api/util.md) | Trapezoidal velocity profile |
| [`slew`](api/util.md) | Rate limiter |
| [`ema`](api/util.md) | Exponential moving average |

---

## Enums

```cpp
vortex::AngularDirection::AUTO            // shortest path (default)
vortex::AngularDirection::CW_CLOCKWISE
vortex::AngularDirection::CCW_COUNTERCLOCKWISE

vortex::DriveSide::LEFT
vortex::DriveSide::RIGHT
```
