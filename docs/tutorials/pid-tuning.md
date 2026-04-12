# PID Tuning & Telemetry

Tuning a PID controller from scratch can be difficult. Vortex makes this easier by providing **Real-time Telemetry**.

## 1. Setup Data Logging

In your `opcontrol` or a dedicated tuning routine, use the `Logger::telemetry` method to send your error or motor power to a serial plotter.

```cpp
void opcontrol() {
    while (true) {
        // Log the current distance error
        vortex::Logger::telemetry("Error", chassis.linear_pid.getError());
        pros::delay(10);
    }
}
```

## 2. Using a Serial Plotter

1.  Connect your V5 Brain to your computer via USB.
2.  Open a serial plotter (like the one built into VSCode, or `v5-serial-telemetry`).
3.  Vortex will output data in the format `>Name:Value`, which most plotters recognize automatically.

## 3. The Tuning Process

### kP (Proportional)
Start with $kI=0$ and $kD=0$. Increase $kP$ until the robot oscillates steadily.

### kD (Derivative)
Increase $kD$ to dampen the oscillations. The robot should reach the target faster without overshooting.

### kI (Integral)
Use a very small $kI$ to correct for tiny steady-state errors. Vortex includes **Integral Anti-Windup** to prevent the robot from "runaway" speed if it gets stuck.

---

## Exit Conditions
Once tuned, set your `ExitCondition`:
```cpp
chassis.linear_pid.setExitCondition({1.0, 50, 2000}); // 1" error, 50ms steady, 2s timeout
```
坐
