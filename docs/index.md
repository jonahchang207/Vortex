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
#include "vortex/vortex.hpp" // High-level unified header

// Setup your configuration with just ports and track width
vortex::Chassis chassis({1, 2, 3}, {-4, -5, -6}, 12.5);

void initialize() {
    chassis.initialize(); // Starts background odometry
}

void autonomous() {
    // Moves to coordinate (10, 20) with a 2000ms timeout
    chassis.moveToPoint({10, 20}, 2000);
}
```
