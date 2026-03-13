# Vortex Odometry Library

A modular odometry and motion control library for VEX Robotics V5 built on PROS.

[![GitHub Release](https://img.shields.io/github/v/release/jonahchang207/Vortex?style=flat-square)](https://github.com/jonahchang207/Vortex/releases/latest)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg?style=flat-square)](LICENSE)

---

## Features

- **Arc Odometry** — Accurate tracking during high-speed turns using circular arc integration
- **Trapezoidal Motion Profiling** — Smooth acceleration and deceleration on every move
- **Full Motion Suite** — `moveToPose`, `moveToPoint`, `turnToHeading`, `swingToHeading`, `follow`
- **Pure Pursuit** — Smooth path following from CSV assets
- **IMU Drift Compensation** — Corrects slow gyro creep while stationary
- **Visual Telemetry** — Live field map + pose overlay on the V5 brain screen
- **SD Card Logging** — CSV telemetry at 10 Hz for post-match analysis
- **Stall Detection** — Automatically cancels a stuck motion
- **Auton Selector** — LLEMU-based routine picker

---

## Installation

### Method 1 — Manual ZIP (recommended)

Download `Vortex.zip` from the [latest release](https://github.com/jonahchang207/Vortex/releases/latest),
place it in your PROS project root, then run:

```bash
pros c fetch Vortex.zip
pros c install Vortex
```

### Method 2 — Direct URL

```bash
pros c fetch https://github.com/jonahchang207/Vortex/releases/download/v1.0.0/Vortex.zip
pros c install Vortex@1.0.0
```

---

## Quick Setup

```cpp
#include "Vortex/api.hpp"

pros::MotorGroup leftMotors ({-1, -2, -3}, pros::MotorGearset::blue);
pros::MotorGroup rightMotors({ 4,  5,  6}, pros::MotorGearset::blue);
pros::Imu        imu(10);
pros::Rotation   vertEnc(-11), horizEnc(20);

vortex::TrackingWheel vertical  (&vertEnc,  vortex::Omniwheel::NEW_275, -2.5);
vortex::TrackingWheel horizontal(&horizEnc, vortex::Omniwheel::NEW_275, -5.75);

vortex::Drivetrain drivetrain(&leftMotors, &rightMotors, 10.0,
                              vortex::Omniwheel::NEW_4, 360, 2.0);

vortex::ControllerSettings linear(10, 0, 3, 3, 1, 100, 3, 500, 20);
vortex::ControllerSettings angular(2, 0, 10, 3, 1, 100, 3, 500, 0);

vortex::OdomSensors sensors(&vertical, nullptr, &horizontal, nullptr, &imu);
vortex::Chassis chassis(drivetrain, linear, angular, sensors);

void initialize() { chassis.calibrate(); }

void autonomous() {
    chassis.setPose(0, 0, 0);
    chassis.moveToPose(24, 24, 90, 4000);
    chassis.follow(my_path_txt, 15, 5000);
}
```

---

## Documentation

Full docs at **[jonahchang207.github.io/Vortex](https://jonahchang207.github.io/Vortex)**

---

## License

MIT — see [LICENSE](LICENSE).
