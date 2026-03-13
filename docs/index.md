# Vortex Odometry Library

Welcome to the documentation for **Vortex**, a high-performance odometry and motion control library for VEX Robotics V5 using PROS.

---

## Current Status: v2.0.6 (Stable)

### What's New in v2.0
- **Arc Odometry** — More accurate tracking during high-speed turns.
- **Pure Pursuit** — Smooth path following with dynamic lookahead.
- **Trapezoidal Motion Profiling** — Automatic smooth acceleration and deceleration.
- **IMU Drift Compensation** — Corrects slow gyro creep while the robot is stationary.
- **Visual Telemetry** — Live field map on the V5 brain screen with motor temperatures and battery.
- **Stall Detection** — Automatically cancels motions when the robot is blocked.

---

## Key Features

- Full 2-wheel + IMU and 3-wheel odometry
- `moveToPose`, `moveToPoint`, `turnToHeading`, `swingToHeading`, `follow`
- PID controllers with slew rate, windup protection, and settle logic
- SD card CSV logging for post-match analysis
- Built-in LLEMU auton selector
- `vortex-cli` for version management and GitHub releases

---

## Get Started

1. [Installation](installation.md) — Add Vortex to your PROS project
2. [Setup Guide](setup.md) — Configure motors and tracking wheels
3. [Tuning Parameters](parameters.md) — Understand `scrubFactor`, `trackWidth`, and PID gains
4. [PID Tuning Guide](guides/tuner.md) — Use built-in tuning functions

---

## Quick Example

```cpp
void autonomous() {
    chassis.setPose(0, 0, 0);
    chassis.moveToPose(24, 24, 90, 4000);
    chassis.follow(my_path_txt, 15, 5000);
}
```

---

*Vortex is open-source — [GitHub](https://github.com/jonahchang207/Vortex)*
