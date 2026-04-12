# Odometry Setup

Odometry is the foundation of precise autonomous. If your odometry is off by 1 inch, your entire auton will drift.

## 1. Physical Layout

To get the best results:
*   **Vertical Wheels**: Should be parallel to the drive wheels. They track your $(y)$ movement.
*   **Horizontal Wheels**: Should be perpendicular. They track your "scrub" (sideways drift) when turning.
*   **IMU**: Place as close to the center of rotation as possible for the best heading data.

---

## 2. Measuring Offsets

The **Offset** is the lateral distance from the center of the robot to the tracking wheel.

*   **Vertical Wheel**: If it is 4 inches to the right of center, offset is `4.0`. If it is 2 inches to the left, offset is `-2.0`.
*   **Horizontal Wheel**: Distance from center to the front/back.

```cpp
// Wheel on the right side, 5 inches from center
auto right_tracker = std::make_shared<vortex::TrackingWheel>(10, 2.75, 5.0);
```

---

## 3. Calibration

1.  Push your robot exactly 48 inches forward.
2.  Check `chassis.odom.getPose().y`.
3.  If it says 46 inches, your **Wheel Diameter** in the config might be slightly off. Adjust it until the screen matches the physical distance exactly.

---

## 4. Multi-Wheel Support

Vortex supports up to 2 vertical and 2 horizontal wheels. 
*   **1 Vertical + IMU**: Best for standard drives.
*   **2 Vertical + 1 Horizontal**: Industry standard for maximum accuracy without an IMU.
*   **3+ Wheels**: Redundancy for high-level competitive robots.
坐
