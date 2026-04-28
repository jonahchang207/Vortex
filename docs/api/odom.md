# Odometry API

Odometry is the process of using sensors to track the robot's position $(x, y)$ and heading $\theta$ on the field relative to its starting position.

## Tracker Wrapper

The `vortex::Tracker` function is the easiest way to create a tracking wheel.

```cpp
std::shared_ptr<TrackingWheel> Tracker(int port, double wheel_diameter, double offset, double gear_ratio = 1.0);
```
*   **port**: PROS smart port of the rotation sensor.
*   **wheel_diameter**: Diameter of the tracking wheel.
*   **offset**: Lateral distance from the center of the drivetrain.
*   **gear_ratio**: External gear ratio (e.g., 36.0/60.0).

---

## OdomConfig

Defines the sensor layout for the `Odom` task.

```cpp
struct OdomConfig {
    std::shared_ptr<TrackingWheel> left_encoder;
    std::shared_ptr<TrackingWheel> right_encoder;
    std::shared_ptr<TrackingWheel> back_encoder;
    std::shared_ptr<TrackingWheel> front_encoder;
    std::shared_ptr<pros::Imu> imu;
};
```

*   **left/right**: Parallel tracking wheels for forward/backward movement.
*   **back/front**: Perpendicular tracking wheels for sideways movement.
*   **IMU**: Inertial sensor for precise heading.

---

## Odom Class

### getPose
Returns the current position and heading.
```cpp
Pose getPose(bool radians = false);
```

### getDistanceTraveled
Returns the total tracked distance since start or reset.

```cpp
double getDistanceTraveled();
```

### resetDistanceTraveled
Resets the tracked distance accumulator.

```cpp
void resetDistanceTraveled();
```

### setPose
Sets the current position and heading. Useful for resetting at the start of autonomous.
```cpp
void setPose(Pose pose);
```

### calibrate
Calibrates the IMU if one is configured.

```cpp
void calibrate();
```

### Pose Struct
```cpp
struct Pose {
    double x;     // Inches
    double y;     // Inches
    double theta; // Degrees (or radians if specified)
};
```
