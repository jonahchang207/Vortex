# PID & Motion Profiles

Vortex includes professional-grade control structures for managing motor power based on sensor error.

## PID

The `vortex::PID` class implements a Proportional-Integral-Derivative controller with anti-windup and exit conditions.

### PIDSettings
```cpp
struct PIDSettings {
    double kP, kI, kD;
    double windup_range; // Integral anti-windup range
    double slew;         // Output slew rate limit
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
*   `reset()`: Clears integral, previous error, and slew state.
*   `setSettings(PIDSettings settings)`: Replace the current PID configuration.
*   `setExitCondition(ExitCondition exit_condition)`: Replace the exit condition.

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
Vortex uses this to clamp PID output in `moveToPoint`, `moveToPose`, and rotational movements. This ensures consistent acceleration and smooth, predictable stopping regardless of how large the target distance is.
