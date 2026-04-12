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

A simple wrapper for `pros::ADIDigitalOut`.

### Methods
*   `extend()`: Sets port to HIGH.
*   `retract()`: Sets port to LOW.
*   `toggle()`: Flips the state.
*   `isExtended()`: Returns true if currently HIGH.

---

## Math Helpers

Efficient math functions specifically for robotics.

*   `wrapAngle(rad)`: Wraps angle to $[-\pi, \pi]$.
*   `wrapAngleDeg(deg)`: Wraps angle to $[-180, 180]$.
*   `getCurvature(pose, x, y)`: Returns circle curvature for path following.
*   `EMA alpha`: Exponential Moving Average filter class.
