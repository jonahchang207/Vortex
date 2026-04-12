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
#include "vortex/drive/chassis.hpp"

// Setup your configuration
vortex::OdomConfig odom_cfg;
vortex::ChassisParams params = {
    {0.5, 0.0, 0.1}, // Linear PID
    {0.3, 0.0, 0.05} // Angular PID
};

vortex::Chassis chassis({nullptr, nullptr, 15.0, odom_cfg}, params);

void initialize() {
    chassis.initialize();
}

void autonomous() {
    // Moves to coordinate (10, 20) with a 2000ms timeout going forward max speed 100
    chassis.moveToPoint({10, 20}, 2000, true, 100);
}
```
