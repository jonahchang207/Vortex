# Vortex Example Project

A ready-to-run PROS project demonstrating the Vortex robotics library.  
Download this alongside the Vortex template from the [Releases page](https://github.com/jonahchang207/Vortex/releases).

---

## What It Shows

| File | Purpose |
|---|---|
| `include/main.h` | The core PROS header, pre-configured with Vortex and your project files |
| `include/subsystems.hpp` | Global declarations of your chassis and motors |
| `src/subsystems.cpp` | Clean configuration of ports, physical specs, and PID tuning |
| `include/autons.hpp` | Declarations of your autonomous routines |
| `src/autons.cpp` | The actual step-by-step logic for your autonomous routines |
| `src/main.cpp` | Driver control loops and competition state hooks |

### Features Demonstrated
- **Clean Split Configuration** — hardware setup is isolated in `subsystems.cpp` away from your main logic
- **Modular Autons** — routines are cleanly separated into `autons.cpp`
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

This `Vortex example progect/` folder is a standalone PROS project. Copy it to your workspace or use it as-is:

```bash
cp -r "Vortex example progect/" ~/my-robot/
cd ~/my-robot/
```

### 3. Edit `src/subsystems.cpp`

Change the motor and sensor ports to match your robot's wiring:

```cpp
auto left_motors = vortex::Motors({1, 2, 3});
auto right_motors = vortex::Motors({-4, -5, -6});

// ...

.left_encoder = vortex::Tracker(11, 3.25, 6.25),
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
