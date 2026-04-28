# Chassis Configuration

Configuring your chassis is the most critical step in setting up Vortex. This tells the library about your motors, gear ratios, and sensors.

## Step 1: Define Motors

In `src/main.cpp`, define your `pros::MotorGroup` objects globally at the top of the file using the `vortex::Motors` wrapper:

```cpp
auto left_motors = vortex::Motors({1, 2, 3});
auto right_motors = vortex::Motors({-4, -5, -6});
```

---

## Step 2: Configure Sensors

Define your tracking wheels and IMU using the `vortex::Tracker` and `vortex::Imu` wrappers. Each wheel needs a port, its physical diameter, and its lateral offset from the center of the robot.

```cpp
auto vertical_wheel = vortex::Tracker(11, 2.75, 0.0);
auto horizontal_wheel = vortex::Tracker(12, 2.75, -4.5);
auto imu = vortex::Imu(20);
```

---

## Step 3: Initialize Chassis

Create the global `Chassis` object using the `ChassisConfig` and `ChassisParams` structs at the top of your `main.cpp`. This keeps your configuration clean and centralized.

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
