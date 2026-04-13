# Vortex Example Project

A ready-to-run PROS project demonstrating the Vortex robotics library.  
Download this alongside the Vortex template from the [Releases page](https://github.com/jonahchang207/Vortex/releases).

---

## What It Shows

| File | Purpose |
|---|---|
| `include/robot.hpp` | All port numbers, PID constants, and physical dimensions in one place |
| `src/main.cpp` | Full robot code — chassis setup, arcade drive, and an autonomous routine |

### Features Demonstrated
- **Chassis + Odometry setup** — motor groups, tracking wheels, IMU
- **Arcade drive** with configurable expo curve
- **`moveToPoint()`** — move to a field coordinate autonomously
- **`turnToHeading()`** — face a specific direction
- **`odom.setPose()`** — set starting pose at the beginning of auto
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

### 3. Edit `include/robot.hpp`

Change the port numbers to match your robot's wiring:

```cpp
constexpr int LEFT_FRONT_PORT  =  1;   // Your actual port
constexpr int RIGHT_FRONT_PORT = -4;   // Negative = reversed
constexpr int IMU_PORT         = 10;
```

Tune the PID constants after your robot drives:

```cpp
constexpr vortex::PIDConstants LINEAR_PID  = { .kP = 8.0, .kI = 0.02, .kD = 1.5 };
constexpr vortex::PIDConstants ANGULAR_PID = { .kP = 3.5, .kI = 0.0,  .kD = 0.8 };
```

### 4. Edit the Autonomous Routine

In `src/main.cpp`, update the starting pose and waypoints to match your field setup:

```cpp
chassis->odom.setPose({ 0.0, 0.0, 90.0 });   // x, y, heading (degrees)
chassis->moveToPoint({ 0.0, 24.0, 90.0 }, 3000);
chassis->turnToHeading(0.0, 1500);
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
