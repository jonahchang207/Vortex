# Odometry Tracking

The Odometry system is responsible for tracking the robot's position and orientation on the field in real-time. It uses a combination of tracking encoders (wheels) and an Inertial Measurement Unit (IMU).

## Odom Class

The `vortex::Odom` class manages the background tracking task and provides coordinate access.

### Constructor

```cpp
Odom(OdomConfig config);
```
Initializes the odometry system with the provided sensor configuration.

### Methods

| Method | Description |
| --- | --- |
| `void start()` | Starts the background tracking task. Should be called once during initialization. |
| `Pose getPose()` | Returns the current position and heading of the robot. |
| `void setPose(Pose pose)` | Manually offsets the robot's coordinates to a specific pose. |

---

## Configuration

Odometry is configured using the `OdomConfig` and `TrackingWheel` structures.

### TrackingWheel

Represents an individual encoder wheel.

```cpp
struct TrackingWheel {
    std::shared_ptr<pros::Rotation> encoder;
    double wheel_diameter;
    double offset; // distance from tracking center
};
```

### OdomConfig

Contains all sensors used for tracking.

```cpp
struct OdomConfig {
    std::shared_ptr<TrackingWheel> left_encoder;
    std::shared_ptr<TrackingWheel> right_encoder;
    std::shared_ptr<TrackingWheel> back_encoder; // Perpendicular
    std::shared_ptr<pros::Imu> imu;
};
```

## Example Usage

```cpp
#include "vortex/drive/odom.hpp"

// Setup encoders
auto left_rot = std::make_shared<pros::Rotation>(1);
auto left_wheel = std::make_shared<vortex::TrackingWheel>(left_rot, 2.75, -5.0);

// Setup config
vortex::OdomConfig config;
config.left_encoder = left_wheel;
// ... add others ...

// Initialize Odom
vortex::Odom odom(config);
odom.start();

// Get position
vortex::Pose current = odom.getPose();
printf("Robot is at: %.2f, %.2f\n", current.x, current.y);
```
