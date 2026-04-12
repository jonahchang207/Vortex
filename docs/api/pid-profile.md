# PID & Motion Profiles

Vortex includes professional-grade control structures for managing motor power based on sensor error.

## PID

The `vortex::PID` class implements a Proportional-Integral-Derivative controller with anti-windup and exit conditions.

### PIDSettings
```cpp
struct PIDSettings {
    double kp, ki, kd;
    double i_max; // Integral anti-windup limit
    double slew;  // Output slew rate limit
};
```

### ExitCondition
Used to determine when a movement is "settled".
```cpp
struct ExitCondition {
    double error_threshold; // In inches or degrees
    int time_threshold;     // Milliseconds error must stay below threshold
    int timeout;            // Max time for the move
};
```

### Methods
*   `update(double error)`: Calculates the motor power.
*   `isSettled(double error)`: Returns true if the exit conditions are met.

---

## TrapezoidalProfile

The `TrapezoidalProfile` class generates a smooth velocity curve (acceleration -> coast -> deceleration).

### Constructor
```cpp
TrapezoidalProfile(double max_speed, double accel, double decel);
```

### calculate
Calculates the target speed for the current distance.
```cpp
double calculate(double distance_to_target, double current_speed);
```
Vortex use this to clamp PID output, ensuring consistent acceleration regardless of how large the target distance is.
