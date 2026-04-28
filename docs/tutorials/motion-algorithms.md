# Motion Algorithms

Vortex features advanced pathing algorithms that go beyond simple point-to-point movements.

## 1. Point and Go

For beginners or simple scripting, `pointAndGo` provides a straightforward way to move to a coordinate. It internally calculates the shortest turn to face the target, turns, and then drives straight to it.

```cpp
// Turns to face (24, 24), then drives to it with a 3000ms total timeout
chassis.pointAndGo(24, 24, 3000);

// You can also do this backwards
chassis.pointAndGo(0, 0, 3000, {.forwards = false});
```

---

## 2. Boomerang (MoveToPose)

The Boomerang controller is designed to move to a point while arriving at a specific angle. It works by projecting a "carrot point" that leads the robot into the target.

```cpp
chassis.moveToPose(24, 24, 90, 2000);
```

### Parameters
*   **Lead Factor**: Controls how much "curvature" the path has. A higher value leads to more drastic turns.
*   **Velocity Profiling**: Boomerang automatically uses **Trapezoidal Profiling** to start and stop smoothly.

---

## 3. Pure Pursuit

Pure Pursuit is the industry standard for following complex, pre-generated paths.

### Using Assets
Vortex treats paths as "Assets" (binary data linked to your project).

```cpp
ASSET(example_path_txt); // Define the path asset
chassis.follow(example_path_txt, 15, 3000); // 15" lookahead
```

### Lookahead Distance
*   **Small Lookahead**: Robot follows the path very tightly but may oscillate.
*   **Large Lookahead**: Robot follows the path more loosely and "cuts corners" for more speed.

---

## 4. Shortest-Path Turning

Vortex turn algorithms automatically calculate the shortest distance to a target angle.

```cpp
// Will turn CCW if target is -90 but CW if target is 270
chassis.turnToHeading(90, 1000, {.direction = vortex::AngularDirection::AUTO});
```
坐
