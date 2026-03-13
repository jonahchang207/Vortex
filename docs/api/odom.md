# Odometry

`#include "Vortex/chassis/odom.hpp"`

`Odom` runs in a 10 ms background task managed by `Chassis`. You don't construct it directly — it's owned by `Chassis` and configured via `OdomSensors`.

---

## Coordinate Convention

| Axis | Direction |
|------|-----------|
| +X | East (right on field at θ=0) |
| +Y | North (forward at θ=0) |
| θ=0 | North |
| θ increases | Clockwise |

---

## Heading Source Priority

1. **Two dedicated (non-motor) vertical tracking wheels** — differential measurement, most accurate
2. **IMU** — single absolute heading, good if only one tracking pod
3. **Motor encoders** — least accurate, only as a last resort

---

## Arc Integration

When the robot turns, `Odom` fits a circular arc to the encoder deltas and computes the chord displacement. This is more accurate than integrating straight-line segments, especially during fast pivots.

```
localX = 2·sin(Δθ/2) · (ΔH/Δθ + hOffset)
localY = 2·sin(Δθ/2) · (ΔV/Δθ + vOffset)
```

When Δθ ≈ 0 (straight line), simplifies to `localX = ΔH`, `localY = ΔV`.

---

## Global Transform

After computing local displacement:

```
pose.x += localY · sin(avgθ) + localX · cos(avgθ)
pose.y += localY · cos(avgθ) − localX · sin(avgθ)
```

where `avgθ` is the heading at the midpoint of the 10 ms interval.

---

## IMU Drift Compensation

When `setDriftCompensation(true)` is enabled, Odom monitors IMU heading while the robot is stationary (all encoder deltas < 0.001 in). It estimates the drift rate using an exponential moving average and subtracts one loop's worth of drift each update.

Enable with:
```cpp
chassis.setImuDriftCompensation(true);
```
