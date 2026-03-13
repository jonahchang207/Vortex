# Tracking Wheels

`#include "Vortex/chassis/trackingWheel.hpp"`

`TrackingWheel` wraps any encoder type into a unified interface that returns **inches traveled**.

---

## Supported Sensors

### `pros::Rotation` (Recommended)
36,000 centidegrees per revolution — highest resolution.

```cpp
pros::Rotation enc(11);  // port 11, forward
pros::Rotation enc(-11); // port 11, reversed (hardware reversal)
vortex::TrackingWheel wheel(&enc, vortex::Omniwheel::NEW_275, -2.5);
```

### `pros::adi::Encoder`
360 ticks per revolution.

```cpp
pros::adi::Encoder enc('A', 'B'); // ADI ports A and B
vortex::TrackingWheel wheel(&enc, vortex::Omniwheel::NEW_275, 5.0);
```

### `pros::MotorGroup`
Uses internal motor encoder in rotations mode. Least accurate — use only when no dedicated sensors are available.

```cpp
vortex::TrackingWheel wheel(&leftMotors, vortex::Omniwheel::NEW_4, -5.0, 360.0);
//                                                                 offset  rpm
```

---

## Offset Sign Convention

- **Vertical wheels** (parallel to drive direction):
  - Positive offset = wheel is **left** of center
  - Negative offset = wheel is **right** of center

- **Horizontal wheels** (perpendicular to drive):
  - Positive offset = wheel is **forward** of center
  - Negative offset = wheel is **behind** center

---

## Omniwheel Constants

```cpp
vortex::Omniwheel::NEW_275   // 2.75" new omni
vortex::Omniwheel::NEW_4     // 4" new omni
vortex::Omniwheel::OLD_4     // 4" old omni (slightly larger)
vortex::Omniwheel::NEW_325   // 3.25" new omni
// ... see trackingWheel.hpp for full list
```

---

## Methods

```cpp
wheel.reset();                 // zero encoder position
wheel.getDistanceTraveled();   // total signed inches since last reset
wheel.getOffset();             // signed center-of-rotation offset
```
