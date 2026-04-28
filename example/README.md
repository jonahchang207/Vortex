# Vortex Example Project

A ready-to-run PROS project demonstrating the Vortex robotics library.  
Download this alongside the Vortex template from the [Releases page](https://github.com/jonahchang207/Vortex/releases).

---

## What It Shows

| File | Purpose |
|---|---|
| `src/main.cpp` | Full robot code — a simple, single-file setup containing your chassis configuration, driver control, and autonomous routines |

### Features Demonstrated
- **Clean Single-File Configuration** — motor groups, tracking wheels, IMU, and PID tuned globally
- **Chassis + Odometry setup** — automatic background initialization
- **Arcade drive** with configurable expo curve
- **`pointAndGo()` & `moveToPose()`** — basic and advanced autonomous pathing
- **LCD debugging** — live pose display during driver control

---

## Quick Start

### 1. Apply the Vortex Template

```bash
pros c fetch vortex@<version>.zip
pros c apply vortex
```

### 2. Open the Example Project

This `example/` folder is a standalone PROS project. Copy it to your workspace or use it as-is:

```bash
cp -r example/ ~/my-robot/
cd ~/my-robot/
```

### 3. Edit `src/main.cpp`

Change the motor and sensor ports to match your robot's wiring directly at the top of the file:

```cpp
auto left_motors = std::make_shared<pros::MotorGroup>(std::initializer_list<int8_t>{1, 2, 3});
auto right_motors = std::make_shared<pros::MotorGroup>(std::initializer_list<int8_t>{-4, -5, -6});
```

Tune the PID constants after your robot drives:

```cpp
    vortex::ChassisParams{
        .linear_pid = {.kP = 8.0, .kI = 0.02, .kD = 1.5},
        .angular_pid = {.kP = 3.5, .kI = 0.0, .kD = 0.8}
    }
```

### 4. Edit the Autonomous Routine

In `src/main.cpp`, update the starting pose and waypoints to match your field setup:

```cpp
chassis.odom.setPose(0.0, 0.0, 90.0);   // x, y, heading (degrees)
chassis.pointAndGo(0.0, 24.0, 3000);
chassis.turnToHeading(0.0, 1500);
```

### 5. Build and Upload

```bash
pros make
pros upload
```

---

## PID Tuning Tips

1. **Start with only `kP`** — zero out `kI` and `kD`
2. **Increase `kP`** until the robot oscillates, then back off ~20%
3. **Add `kD`** to dampen the oscillation
4. **Add a tiny `kI`** only if there is consistent steady-state error

---

## Further Reading

- [Vortex Docs](https://jonahchang207.github.io/Vortex/)
- [PROS Documentation](https://pros.cs.purdue.edu/)
