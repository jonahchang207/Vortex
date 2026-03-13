# Chassis

`#include "Vortex/chassis/chassis.hpp"`

The `Chassis` class is the top-level controller. It owns odometry, PID loops, background tasks, and every motion primitive.

---

## Constructor

```cpp
vortex::Chassis chassis(drivetrain, linearSettings, angularSettings, sensors);
// With custom drive curves:
vortex::Chassis chassis(drivetrain, linear, angular, sensors, &throttleCurve, &steerCurve);
```

---

## Setup

### `calibrate(bool calibrateImu = true)`
Calibrates the IMU (up to 5 retries), then starts background tasks:
- Odometry (10 ms)
- Brain screen telemetry (20 ms)
- Stall detection (10 ms)
- SD card logging (100 ms)

Call once in `initialize()`.

### `calibratePhysical()`
Spins the robot 10 full rotations and prints suggested `scrubFactor` and `trackWidth`. Run before PID tuning.

---

## Odometry

### `setPose(float x, float y, float theta, bool radians = false)`
Set the robot's current position. Theta is degrees by default.

### `getPose(bool radians = false, bool standardPos = false)`
Returns the current `Pose`. `standardPos=true` converts to math-standard CCW-from-east convention.

---

## Motions

All motions accept an `async` flag. When `true` the function returns immediately and the motion runs in a background task.

### `moveToPose(x, y, theta, timeout, params, async)`
Drives to `(x, y)` and arrives facing `theta` degrees. Uses a carrot-point approach for smooth curves.

```cpp
chassis.moveToPose(24, 24, 90, 4000);
chassis.moveToPose(-12, 0, 180, 3000, {.forwards=false, .lead=0.4});
```

### `moveToPoint(x, y, timeout, params, async)`
Drives to `(x, y)` without a final heading constraint. Uses a trapezoidal motion profile.

```cpp
chassis.moveToPoint(24, 0, 2000);
chassis.moveToPoint(0, -24, 2000, {.forwards=false});
```

### `turnThenMove(x, y, timeout, params, async)`
Turns to face `(x, y)` then drives straight to it. More accurate than `moveToPoint` when the robot starts badly misaligned.

### `turnToHeading(heading, timeout, params, async)`
Turns in-place to an absolute heading (degrees).

```cpp
chassis.turnToHeading(90, 1500);
chassis.turnToHeading(270, 2000, {.direction=AngularDirection::CCW_COUNTERCLOCKWISE});
```

### `turnToPoint(x, y, timeout, params, async)`
Turns in-place to face a field coordinate.

### `swingToHeading(heading, side, timeout, params, async)`
Turns with only one side of the drivetrain, pivoting around the locked wheel.

```cpp
chassis.swingToHeading(90, vortex::DriveSide::LEFT, 1500);
```

### `follow(path, lookahead, timeout, forwards, async)`
Pure Pursuit path following. `path` is a `ASSET()` macro pointing to a CSV file in `static/`.

```cpp
ASSET(my_path_txt); // static/my_path.txt
chassis.follow(my_path_txt, 15, 5000);
```

---

## Tuning

### `tuneLateral(dist, timeout)`
Runs a lateral PID tuning motion and prints `TUNE_LAT: time,error,power` CSV to the terminal.

### `tuneAngular(heading, timeout)`
Runs an angular PID tuning motion and prints `TUNE_ANG: time,error,power` CSV.

---

## Driver Control

### `tank(left, right, disableDriveCurve)`
### `arcade(throttle, turn, disableDriveCurve, desaturateBias)`
### `curvature(throttle, turn, disableDriveCurve)`

---

## Waiting

```cpp
chassis.waitUntil(12);             // wait until 12 inches into the current motion
chassis.waitUntilPose(24, 24, 3);  // wait until within 3 inches of (24, 24)
chassis.waitUntilDone();           // wait until motion is fully complete
chassis.waitUntilDone(2000);       // same, but with a hard 2-second timeout
```

---

## Configuration

```cpp
chassis.setBrakeMode(pros::E_MOTOR_BRAKE_HOLD);
chassis.setStallDetection(10.0, 250);       // cancel if < 10 RPM for 250 ms under load
chassis.setLogging(true, "/usd/log.csv");   // SD card telemetry
chassis.setFieldRelative(true);             // arcade always moves field-north
chassis.setImuDriftCompensation(true);      // correct slow gyro creep
```

---

## State

```cpp
chassis.isInMotion()    // true during any autonomous motion
chassis.isStalled()     // true if stall was detected this motion
chassis.cancelMotion()  // abort the running motion
chassis.cancelAllMotions()
```
