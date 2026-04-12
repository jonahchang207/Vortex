# Odometry API

Odometry is the process of using sensors to track the robot's position $(x, y)$ and heading $\theta$ on the field relative to its starting position.

## TrackingWheel

The `TrackingWheel` class represents a physical wheel used for tracking.

### Constructor
```cpp
TrackingWheel(int port, double wheel_diameter, double offset, double gear_ratio = 1.0);
```
*   **port**: Motor or Rotation sensor port.
*   **wheel_diameter**: Diameter of the tracking wheel.
*   **offset**: Lateral distance from the center of the drivetrain.
*   **gear_ratio**: External gear ratio (e.g., 36.0/60.0).

---

## OdomConfig

Defines the sensor layout for the `Odom` task.

```cpp
struct OdomConfig {
    std::shared_ptr<TrackingWheel> vertical1;
    std::shared_ptr<TrackingWheel> vertical2;
    std::shared_ptr<TrackingWheel> horizontal1;
    std::shared_ptr<TrackingWheel> horizontal2;
    std::shared_ptr<pros::Imu> imu;
};
```

*   **Vertical**: Tracks forward/backward movement.
*   **Horizontal**: Tracks side-to-side (strafing/scrub) movement.
*   **IMU**: Inertial sensor for precise heading.

---

## Odom Class

### getPose
Returns the current position and heading.
```cpp
Pose getPose(bool radians = false);
```

### setPose
Sets the current position and heading. Useful for resetting at the start of autonomous.
```cpp
void setPose(Pose pose);
```

### Pose Struct
```cpp
struct Pose {
    double x;     // Inches
    double y;     // Inches
    double theta; // Degrees (or radians if specified)
};
```
