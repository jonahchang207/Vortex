# Changelog

See [CHANGELOG.md](https://github.com/jonahchang207/Vortex/blob/stable/CHANGELOG.md) on GitHub for the full history.

## [2.0.6] — 2026

### Bug Fixes

**Critical — Odometry X axis was inverted**
The global transform applied `−cos(θ)` and `+sin(θ)` on the lateral wheel term, which caused moving right to decrease X instead of increasing it and introduced phantom drift in every turn. The correct rotation matrix is `x += localY·sin(θ) + localX·cos(θ)`.

**Medium — `turnToPoint` double-converted angle error**
`angleError(…, radians=false)` already returns degrees, but the result was then passed through `radToDeg()` again — multiplying by 57.3 a second time and making every turn target ~57× too small.

**Medium — `Timer` accumulated time on every getter call**
The original design added `time − lastTime` to `timeWaited` inside every call to `getTimeLeft()`, `getTimePassed()`, and `isDone()`. Calling any of them more than once per loop would accumulate extra time. Replaced with a start-time snapshot: `elapsed = millis() − startTime`, which is idempotent.

**Medium — `moveToPoint` motion profiler saw a fluctuating total distance**
`totalDist` was recalculated every loop iteration. As the robot curves, the sum of `currentDistToTarget + distTraveled` fluctuates, making the profiler compute an unstable velocity ceiling. Now captured once before the loop starts.

**Medium — `follow()` fell back to a stale lookahead point**
When no circle/segment intersection was found, the function reused `lastLookaheadPoint` — a stale value potentially from early in the path. Replaced with a forward-scanning closest-point fallback using a monotonically advancing segment index.

**Low — `getCurvature` overflowed at θ=π/2**
The original formula used `tan(θ)`, which produces ±∞ when the robot faces exactly east or west. Rewrote using the full `sin(θ)·x − cos(θ)·y` line-equation form, which is finite at all headings.

**Low — Float equality on `horizontalDrift`**
`params.horizontalDrift == 0` used exact float comparison. Changed to `< 1e-6`.

**CLI — Version bump never ran on plain invocation**
`apply_version()` was inside the `if VERSION_CHANGED` block, which was only set by the manual `-v` flag. Running `./vortex-cli` always skipped it. Now `apply_version()` always runs before the release/dev split.

### Changed

- `Timer` fully rewritten with `startTime` snapshot — all getters are now idempotent.
- `getCurvature` rewritten without `tan()`.
- `follow()` lookahead now scans forward monotonically — can't latch to past segments.
- `pros::Task` in `Buffer` moved to a heap pointer — `pros::Task` has no default constructor.
- `mutex.take()` calls throughout now pass `TIMEOUT_MAX` explicitly (PROS4 requirement).
- `pros::screen::print` updated to `pros::E_TEXT_MEDIUM` enum (PROS4 naming).
- Velocity estimation in `odom.cpp` moved from a function-local static to a class member — correct for multi-instance use.

## [2.0.0] — 2025

- Initial arc odometry, Pure Pursuit, trapezoidal profiling, IMU drift compensation, SD logging, `vortex-cli`

## [1.x] — 2024

- LemLib-derived odometry and PID chassis
