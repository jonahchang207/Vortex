# Chassis Configuration

Configuring your chassis is the most critical step in setting up Vortex. This tells the library about your motors, gear ratios, and sensors.

## Step 1: Define Motors

In `main.cpp` (or a dedicated configuration file), define your `pros::MotorGroup` objects:

```cpp
auto left_motors = std::make_shared<pros::MotorGroup>({1, -2, 3});
auto right_motors = std::make_shared<pros::MotorGroup>({-4, 5, -6});
```

---

## Step 2: Configure Sensors

Define your tracking wheels. Each wheel needs a port (Motor or Rotation Sensor), its diameter, and its lateral offset from the center.

```cpp
auto vertical_wheel = std::make_shared<vortex::TrackingWheel>(10, 2.75, 0.0);
auto horizontal_wheel = std::make_shared<vortex::TrackingWheel>(11, 2.75, -4.5);
auto imu = std::make_shared<pros::Imu>(20);

vortex::OdomConfig odom_sensors = {
    .vertical1 = vertical_wheel,
    .horizontal1 = horizontal_wheel,
    .imu = imu
};
```

---

## Step 3: Initialize Chassis

Create the `Chassis` object using the `ChassisConfig` struct.

```cpp
vortex::Chassis chassis({
    .left_motors = left_motors,
    .right_motors = right_motors,
    .track_width = 10.5,
    .wheel_diameter = 3.25,
    .rpm = 450,
    .odom_sensors = odom_sensors
});

void initialize() {
    chassis.initialize(); // Starts the odometry task
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
