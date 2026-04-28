# Welcome to Vortex

Vortex is a powerful Odometry and Motion Control template designed specifically for VEX Robotics Competition (V5) using the PROS Library.

## Features
- **Easy Configurable Tracking**: Quick setup for external tracking wheels and IMU.
- **Robust PID Controllers**: Easy tuning for optimal performance.
- **Odometry Tracking**: Background position tracking mapping exactly to standard X, Y, Theta coordinates.
- **Motion Algorithms**: Clean, non-blocking motion functions with curve scaling and auto-deadbanding.
- **Driver Control**: Built in smooth tank and arcade abstractions.

## Quick Start
```cpp
#include "main.h"
#include "vortex/vortex.hpp"

auto left = std::make_shared<pros::MotorGroup>(std::vector<int8_t>{1, 2, 3});
auto right = std::make_shared<pros::MotorGroup>(std::vector<int8_t>{-4, -5, -6});

auto imu = std::make_shared<pros::Imu>(10);

vortex::Chassis chassis(
    {
        .left_motors = left,
        .right_motors = right,
        .track_width = 12.5,
        .wheel_diameter = 4.0,
        .rpm = 600,
        .odom_config = {
            .imu = imu
        }
    },
    {
        .linear_pid = {.kP = 8.0, .kI = 0.0, .kD = 1.0},
        .angular_pid = {.kP = 4.0, .kI = 0.0, .kD = 0.5}
    }
);

void initialize() {
    chassis.initialize();
}

void autonomous() {
    chassis.moveToPoint(10, 20, 2000);
}
```
