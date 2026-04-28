# Utilities API

Vortex provides several small but powerful utilities to simplify your code.

## Logger & Telemetry

The `Logger` class provides professional logging to the terminal and telemetry for PID plotting.

### Methods
*   `V_LOG_INFO(sender, msg)`: Log info message.
*   `V_LOG_DEBUG(sender, msg)`: Log debug message.
*   `Logger::telemetry(name, value)`: Send data to a serial plotter. Format: `>name:value`.

---

## Timer

A non-blocking timer for multi-tasking.

### Example
```cpp
vortex::Timer my_timer;
while (!my_timer.hasPassed(1000)) {
    // This runs for 1 second while your main code stays responsive
}
```

---

## Piston (Pneumatics)

A simple wrapper for `pros::adi::DigitalOut`.

### Methods
*   `extend()`: Sets port to HIGH.
*   `retract()`: Sets port to LOW.
*   `toggle()`: Flips the state.
*   `set(bool state)`: Set the piston state directly.
*   `get()`: Returns the cached extended/retracted state.

---

## Math Helpers

Efficient math functions specifically for robotics.

*   `wrapAngle(rad)`: Wraps angle to $[-\pi, \pi]$.
*   `wrapAngleDeg(deg)`: Wraps angle to $[-180, 180]$.
*   `getCurvature(pose, x, y)`: Returns circle curvature for path following.
*   `ExpoDriveCurve(deadband, min_output, curve)`: Exponential joystick response helper.
*   `LinearDriveCurve(deadband)`: Simple deadband-only joystick response helper.

---

## GUI Utilities

### AutonSelector

Stores autonomous routines and tracks the selected routine and alliance.

### Dashboard

Provides an LVGL-based dashboard for displaying robot pose and selecting autonomous routines.
