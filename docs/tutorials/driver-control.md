# Driver Control

Vortex provides professional tools for customizing how your robot feels to the driver.

## Drive Curves

Linear joystick mapping often makes fine adjustments difficult. Vortex includes **Exponential Curves** to solve this.

### Why use a Curve?
A curve scales the joystick input so that moving the stick 50% only applies maybe 25% power, but moving it to 100% still applies 100% power. This gives you high precision at low speeds.

```cpp
void opcontrol() {
    while (true) {
        // Apply arcade drive with an expo curve of 2.0
        chassis.arcade(master.get_analog(ANALOG_LEFT_Y), 
                       master.get_analog(ANALOG_RIGHT_X), 
                       2.0);
        pros::delay(10);
    }
}
```

### Split Arcade
If you prefer to have the throttle and turn on separate joysticks (e.g., Left stick for Forward/Back, Right stick for Turn), use `arcadeSplit`:

```cpp
void opcontrol() {
    while (true) {
        // Left stick Y for forward, Right stick X for turn
        chassis.arcadeSplit(master.get_analog(ANALOG_LEFT_Y), 
                            master.get_analog(ANALOG_RIGHT_X));
        pros::delay(10);
    }
}
```

---

## Steering Desaturation

In standard arcade drive, if you are driving forward at 100% and try to turn at 100%, one motor would need to be 200% power (impossible). Usually, the code just clips the output, causing you to lose your turn.

Vortex uses **Desaturation** to prioritize your steering. If the motors are saturated, it scales down the forward speed to ensure the side-to-side difference (the turn) is preserved.

---

## Brake Modes

Set how the robot behaves when joysticks are released:

```cpp
chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE); // Locks wheels
chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST); // Rolls to stop
```
坐
