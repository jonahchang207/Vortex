# Chassis API

The `vortex::Chassis` class is the primary interface for controlling the robot's movement. It handles everything from driver control to advanced Boomerang and Pure Pursuit motions.

## Class Definition

### Constructor

```cpp
Chassis(ChassisConfig config, ChassisParams params);
```

Initializes the chassis. After construction, call `initialize()` to start background tasks.

### Driver Control

| Method | Description |
| --- | --- |
| `void tank(int left, int right, double curve = 0.0)` | Tank drive with optional exponential curve scaling. |
| `void arcade(int forward, int turn, double curve = 0.0)` | Arcade drive with **Steering Desaturation** (prioritizes turns at full speed). |
| `void arcadeSplit(int forward, int turn, double curve = 0.0)` | Split Arcade drive (throttle and turn on different joysticks). |
| `void curvature(int throttle, int turn, double curve = 0.0)` | Curvature drive with optional exponential curve scaling. |

### PTO Management

| Method | Description |
| --- | --- |
| `void setPTO(std::shared_ptr<pros::MotorGroup> left, std::shared_ptr<pros::MotorGroup> right)` | Temporarily swap the active drive motor groups. |
| `void resetPTO()` | Restore the original drive motor groups. |

### Autonomous Motions

All autonomous motions can be called synchronously (blocks until done) or asynchronously by setting `.async = true` in the parameter struct.

#### moveToPoint
Moves the robot to a coordinate $(x, y)$.

```cpp
void moveToPoint(double x, double y, int timeout, MoveToPointParams params = {});
```

#### pointAndGo
Sequentially turns to face a point and then drives to it. Ideal for basic autonomous scripting.

```cpp
void pointAndGo(double x, double y, int timeout, PointAndGoParams params = {});
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

#### turnToPoint
Turns to face a target coordinate.

```cpp
void turnToPoint(double x, double y, int timeout, TurnParams params = {});
```

#### swingToHeading
Locks one side of the drivetrain and swings to an absolute heading.

```cpp
void swingToHeading(double theta, DriveSide side, int timeout, SwingParams params = {});
```

#### swingToPoint
Locks one side of the drivetrain and swings to face a point.

```cpp
void swingToPoint(double x, double y, DriveSide side, int timeout, SwingParams params = {});
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
| `min_speed` | `0` | Minimum motor power for motion chaining. |
| `accel` | `5.0` | Slew rate / Acceleration limit. |
| `decel` | `5.0` | Deceleration limit for profile. |
| `early_exit_range` | `0` | Exit distance if chained. |
| `async` | `false` | Run the motion in the background. |

### MoveToPoseParams
| Field | Default | Description |
| --- | --- | --- |
| `lead` | `0.6` | How far the carrot point leads (0.0 to 1.0). |
| `max_speed` | `127` | Maximum motor power. |
| `min_speed` | `0` | Minimum motor power for motion chaining. |
| `accel` | `5.0` | Profile acceleration. |
| `decel` | `5.0` | Profile deceleration. |
| `early_exit_range` | `0` | Exit early when within range. |
| `async` | `true` | Run the motion in the background. |

### PointAndGoParams
| Field | Default | Description |
| --- | --- | --- |
| `forwards` | `true` | Whether to drive forward or backward. |
| `max_speed` | `127` | Maximum motor power. |
| `min_speed` | `0` | Minimum motor power. |
| `accel` | `5.0` | Profile acceleration. |
| `decel` | `5.0` | Profile deceleration. |
| `early_exit_range` | `0` | Exit distance if chained. |
| `async` | `false` | Run the motion in the background. |

### TurnParams
| Field | Default | Description |
| --- | --- | --- |
| `direction` | `AUTO` | `AUTO`, `CW_CLOCKWISE`, or `CCW_COUNTERCLOCKWISE`. |
| `max_speed` | `127` | Maximum motor power. |
| `min_speed` | `0` | Minimum motor power. |
| `accel` | `5.0` | Profile acceleration. |
| `decel` | `5.0` | Profile deceleration. |
| `async` | `true` | Run the motion in the background. |

### SwingParams
| Field | Default | Description |
| --- | --- | --- |
| `left_side` | `true` | Whether to move the left or right side motors. |
| `direction` | `AUTO` | `AUTO`, `CW_CLOCKWISE`, or `CCW_COUNTERCLOCKWISE`. |
| `max_speed` | `127` | Maximum motor power. |
| `min_speed` | `0` | Minimum motor power. |
| `accel` | `5.0` | Profile acceleration. |
| `decel` | `5.0` | Profile deceleration. |
| `async` | `true` | Run the motion in the background. |

---

## Wait Helpers

Use these to coordinate mechanism actions during or after movements.

| Method | Description |
| --- | --- |
| `void waitUntilDist(double inches)` | Pause code until within X inches of target. |
| `void waitUntilAngle(double degrees)` | Pause code until within X degrees of target. |
| `void waitUntilSettled()` | Pause code until the movement settles. |
| `void waitUntilDone()` | Pause code until the movement task fully finishes. |
