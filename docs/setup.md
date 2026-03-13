# Setup Guide

## 1. Physical Measurements

Measure your robot before writing any code.

| Value | Description |
|-------|-------------|
| **Track Width** | Distance between left and right wheel centerlines (inches) |
| **Wheel Diameter** | Actual drive wheel diameter — use `Omniwheel::NEW_4`, `NEW_275`, etc. |
| **RPM** | Output shaft speed after gearing (e.g. 360 for a 36:60 blue cartridge) |
| **Horizontal Drift** | Lateral friction coefficient — start with `2.0` |
| **Tracking Wheel Offset** | Signed perpendicular distance from center of rotation to wheel contact (inches) |

---

## 2. Sensors

### Rotation Sensor (recommended)

```cpp
pros::Rotation vertEnc(-11);  // negative port = physically reversed
vortex::TrackingWheel vertical(&vertEnc, vortex::Omniwheel::NEW_275, -2.5);
// 2.75" wheel, 2.5" to the right of center (negative = right)
```

### ADI Encoder

```cpp
pros::adi::Encoder adiEnc('A', 'B');
vortex::TrackingWheel horizontal(&adiEnc, vortex::Omniwheel::NEW_275, -5.75);
```

### IMU

```cpp
pros::Imu imu(10);
```

---

## 3. OdomSensors

Pass `nullptr` for any sensor you don't have. Minimum: one vertical wheel + IMU, OR two vertical wheels.

```cpp
vortex::OdomSensors sensors(
    &vertical,    // vertical tracking wheel 1  (required)
    nullptr,      // vertical tracking wheel 2  (optional)
    &horizontal,  // horizontal tracking wheel  (optional, improves strafe tracking)
    nullptr,      // second horizontal          (optional)
    &imu          // IMU                        (optional if two vertical pods)
);
```

---

## 4. ControllerSettings

```cpp
vortex::ControllerSettings linear(
    10,   // kP — proportional gain
     0,   // kI — integral gain (use sparingly)
     3,   // kD — derivative gain
     3,   // windupRange — integral only accumulates within this error (inches)
     1,   // smallError — "settled" threshold (inches)
   100,   // smallErrorTimeout — ms to stay in smallError before exiting
     3,   // largeError — "close enough" threshold (inches)
   500,   // largeErrorTimeout — hard motion timeout (ms)
    20    // slew — max power change per 10 ms loop
);
```

---

## 5. Chassis

```cpp
vortex::Chassis chassis(drivetrain, linear, angular, sensors);

void initialize() {
    chassis.calibrate(); // calibrates IMU, starts odometry + screen tasks
}
```
