# Configuration Reference

## Drivetrain

```cpp
vortex::Drivetrain drivetrain(
    &leftMotors,              // pros::MotorGroup*
    &rightMotors,             // pros::MotorGroup*
    10.0,                     // track width (inches)
    vortex::Omniwheel::NEW_4, // wheel diameter (inches)
    360,                      // output RPM
    2.0,                      // horizontal drift coefficient
    1.0                       // scrub factor (tune with calibratePhysical())
);
```

## ControllerSettings

Both lateral and angular controllers use the same struct.

```cpp
vortex::ControllerSettings(kP, kI, kD, windupRange,
                            smallError, smallErrorTimeout,
                            largeError, largeErrorTimeout,
                            slew, maxVelocity=127, maxAccel=127);
```

## OdomSensors

```cpp
vortex::OdomSensors sensors(&v1, &v2, &h1, &h2, &imu);
```

Any pointer can be `nullptr`. Heading priority: two tracking pods → IMU → motor encoders.

## Chassis

```cpp
vortex::Chassis chassis(drivetrain, linearSettings, angularSettings, sensors);
// Optional: custom drive curves
vortex::Chassis chassis(drivetrain, linear, angular, sensors, &throttleCurve, &steerCurve);
```

## Optional Features

```cpp
chassis.setStallDetection(10.0, 250);     // cancel if velocity < 10 RPM for 250 ms
chassis.setLogging(true, "/usd/log.csv"); // SD card telemetry at 10 Hz
chassis.setFieldRelative(true);           // arcade stick always points field-north
chassis.setImuDriftCompensation(true);    // correct IMU drift while stationary
chassis.setBrakeMode(pros::E_MOTOR_BRAKE_HOLD);
```
