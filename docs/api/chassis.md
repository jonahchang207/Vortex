# Chassis API

The `vortex::Chassis` class is the primary interface for controlling the robot's movement. It handles everything from driver control to advanced Boomerang and Pure Pursuit motions.

## Class Definition

### Constructor

```cpp
Chassis(ChassisConfig config);
```

Initializes the chassis. After construction, call `initialize()` to start background tasks.

### Driver Control

| Method | Description |
| --- | --- |
| `void tank(int left, int right, double curve = 0.0)` | Tank drive with optional exponential curve scaling. |
| `void arcade(int forward, int turn, double curve = 0.0)` | Arcade drive with **Steering Desaturation** (prioritizes turns at full speed). |

### Autonomous Motions

All autonomous motions can be called synchronously (blocks until done) or asynchronously by setting `.async = true` in the parameter struct.

#### moveToPoint
Moves the robot to a coordinate $(x, y)$.

```cpp
void moveToPoint(double x, double y, int timeout, MoveToPointParams params = {});
```

#### moveToPose (Boomerang)
Moves to a target $(x, y)$ while arriving at a specific heading $\theta$. Uses the **Boomerang** carrot-point algorithm and **Trapezoidal Velocity Profiling**.

```cpp
void moveToPose(double x, double y, double theta, int timeout, MoveToPoseParams params = {});
```

#### turnToHeading
Turns to an absolute heading.

```cpp
void turnToHeading(double theta, int timeout, TurnParams params = {});
```

#### follow (Pure Pursuit)
Follows a path generated as an `Asset`.

```cpp
void follow(Asset path, double lookahead, int timeout, bool forwards = true, bool async = true);
```

---

## Parameter Structs

### MoveToPointParams
| Field | Default | Description |
| --- | --- | --- |
| `forwards` | `true` | Whether to drive forward or backward. |
| `max_speed` | `127` | Maximum motor power. |
| `accel` | `5.0` | Slew rate / Acceleration limit. |
| `decel` | `5.0` | Deceleration limit for profile. |
| `early_exit_range` | `0` | Exit distance if chained. |

### MoveToPoseParams
| Field | Default | Description |
| --- | --- | --- |
| `lead` | `0.6` | How far the carrot point leads (0.0 to 1.0). |
| `max_speed` | `127` | Maximum motor power. |
| `accel` | `5.0` | Profile acceleration. |
| `decel` | `5.0` | Profile deceleration. |

---

## Wait Helpers

Use these to coordinate mechanism actions during or after movements.

| Method | Description |
| --- | --- |
| `void waitUntilDist(double inches)` | Pause code until within X inches of target. |
| `void waitUntilAngle(double degrees)` | Pause code until within X degrees of target. |
| `void waitUntilDone()` | Pause code until movement is fully finished. |
