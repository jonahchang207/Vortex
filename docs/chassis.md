# Chassis Control

The `vortex::Chassis` class is the primary interface for controlling the robot's movement, providing both driver control (tank/arcade) and autonomous movement algorithms.

## Chassis Class

### Constructor

```cpp
Chassis(ChassisConfig config, ChassisParams params);
```
Initializes the chassis with motor groups and PID constants.

### Driver Control

| Method | Description |
| --- | --- |
| `void tank(int left, int right, double curve)` | Standard tank drive with optional exponential curve scaling. |
| `void arcade(int forward, int turn, double curve)` | Standard arcade drive with optional curve scaling. |

### Autonomous Methods

#### moveToPoint
Moves the robot to a target coordinate using a basic 2D PID controller.

```cpp
void moveToPoint(Pose target, int timeout, bool forward = true, int max_speed = 127, int min_speed = 0);
```

#### turnToHeading
Turns the robot to a specific absolute heading.

```cpp
void turnToHeading(double angle, int timeout, int max_speed = 127, int min_speed = 0);
```

---

## Configuration

### ChassisConfig

```cpp
struct ChassisConfig {
    std::shared_ptr<pros::MotorGroup> left_motors;
    std::shared_ptr<pros::MotorGroup> right_motors;
    double track_width; // Distance between wheels
    OdomConfig odom_config;
};
```

### ChassisParams

Contains PID constants used for autonomous movements.

```cpp
struct ChassisParams {
    PIDConstants linear_pid;
    PIDConstants angular_pid;
};
```

## Example Configuration

```cpp
// Define motors
auto left = std::make_shared<pros::MotorGroup>({1, 2, 3});
auto right = std::make_shared<pros::MotorGroup>({-4, -5, -6});

// Setup Chassis
vortex::ChassisConfig config = {left, right, 10.5, odom_config};
vortex::ChassisParams params = {
    {1.5, 0.01, 0.1}, // Linear PID (kP, kI, kD)
    {0.8, 0.00, 0.05}  // Angular PID
};

vortex::Chassis chassis(config, params);

void opcontrol() {
    while (true) {
        chassis.arcade(controller.get_analog(ANALOG_LEFT_Y), controller.get_analog(ANALOG_RIGHT_X));
        pros::delay(10);
    }
}
```
