# Utility Functions

`#include "Vortex/util.hpp"`

---

## Angle Utilities

### `angleError(target, position, radians, direction)`
Signed angular error from `position` to `target`. Handles wrapping correctly.

```cpp
double err = vortex::angleError(90.0, 350.0, false);
// Returns 100.0 (shortest path from 350° to 90° is +100° CCW)

double err = vortex::angleError(90.0, 350.0, false, AngularDirection::CW_CLOCKWISE);
// Returns -260.0 (forced CW route)
```

### `degToRad(deg)` / `radToDeg(rad)`
Unit conversions. Constexpr.

### `sanitizeAngle(angle, radians)`
Normalizes to `[0, 2π)` or `[0, 360)`.

### `normalizeAngle(angle)`
Normalizes to `(−π, π]`.

---

## Math Helpers

### `slew(target, current, maxStep)`
Rate limiter. Limits how much `current` can change toward `target` in one step.

```cpp
power = vortex::slew(targetPower, lastPower, 15.0); // max change 15/loop
```

### `deadband(value, threshold)`
Returns 0 if `|value| < threshold`, otherwise returns `value` unchanged.

### `ema(current, previous, smooth)`
Exponential moving average. `smooth=1.0` → fully current; `smooth=0.0` → fully previous.

### `avg(values)`
Average of a `std::vector<double>`. Returns 0 for empty input.

### `sgn(v)`
Returns +1 or -1. Works on any numeric type.

---

## Geometry

### `getCurvature(pose, other)`
Signed curvature of the circle passing through `pose` (as a directed point) and `other`. Positive = turn left (CCW), negative = turn right (CW). Safe at all headings — does not use `tan(θ)`.

### `lineCircleIntersection(p1, p2, center, radius)`
Returns the intersection points of the segment `p1→p2` with a circle. Returns empty if no intersection lies on the segment.

---

## Motion Profile

### `motionProfile(dist, totalDist, maxVel, maxAccel, currentVel)`
Trapezoidal velocity profile. Returns the velocity ceiling at position `dist` along a move of total length `totalDist`.

- Accelerates from rest: `v = sqrt(2·a·d)`
- Decelerates to rest: `v = sqrt(2·a·(total − d))`
- Clamped to `maxVel`
