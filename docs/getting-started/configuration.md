# Chassis Configuration

Configuring your chassis is the most critical step in setting up Vortex. This tells the library about your motors, gear ratios, and sensors.

## Step 1: Define Config Header

To keep your code clean and prevent your `main.cpp` from getting cluttered, we recommend defining your hardware in a dedicated `subsystems.cpp` and exposing it via `subsystems.hpp`.

**`include/subsystems.hpp`**:
```cpp
#pragma once
#include "vortex/vortex.hpp"
#include "pros/motor_group.hpp"

// Expose motors and chassis globally
extern std::shared_ptr<pros::MotorGroup> left_motors;
extern std::shared_ptr<pros::MotorGroup> right_motors;
extern vortex::Chassis chassis;
```

---

## Step 2: Define Motors & Sensors

In your **`src/subsystems.cpp`**, define your `pros::MotorGroup` objects and sensors using Vortex's clean wrappers.

```cpp
#include "main.h"

auto left_motors = vortex::Motors({1, 2, 3});
auto right_motors = vortex::Motors({-4, -5, -6});

auto vertical_wheel = vortex::Tracker(11, 2.75, 0.0);
auto horizontal_wheel = vortex::Tracker(12, 2.75, -4.5);
auto imu = vortex::Imu(20);
```

---

## Step 3: Initialize Chassis

Still in **`src/subsystems.cpp`**, create the global `Chassis` object using the `ChassisConfig` and `ChassisParams` structs.

```cpp
vortex::Chassis chassis(
    vortex::ChassisConfig{
        .left_motors = left_motors,
        .right_motors = right_motors,
        .track_width = 10.5,
        .wheel_diameter = 3.25,
        .rpm = 450,
        .odom_config = {
            .left_encoder = vertical_wheel,
            .right_encoder = nullptr, // Or your right wheel
            .back_encoder = horizontal_wheel,
            .imu = imu
        }
    },
    vortex::ChassisParams{
        .linear_pid = {.kP = 8.0, .kI = 0.0, .kD = 1.0},
        .angular_pid = {.kP = 4.0, .kI = 0.0, .kD = 0.5}
    }
);
```

Finally, in **`src/main.cpp`**, start the chassis in `initialize()`:

```cpp
#include "main.h"

void initialize() {
    chassis.initialize(); // Starts the odometry task and background processes
}
```

---

## Advanced Options

You can also customize the **Drive Curves** for joystick control:

```cpp
chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE);
// Use an exponential curve with a 2.0 power factor
chassis.arcade(y, x, 2.0); 
```
坐
