# Changelog

All notable changes are documented here.

## [2.0.6] — 2026
### Fixed
- **Critical:** Odometry global transform had inverted X signs — moving right decreased X
  instead of increasing it, breaking all X-axis tracking.
- **Medium:** `turnToPoint` double-converted angle error through `radToDeg()` on a value
  already in degrees, making all turn targets ~57× smaller than intended.
- **Medium:** `Timer` accumulated `timeWaited` multiple times per real millisecond when
  any getter was called more than once per loop — replaced with snapshot approach.
- **Medium:** `moveToPoint` recalculated `totalDist` every loop iteration, making the
  motion profiler see a fluctuating total distance — now captured once before the loop.
- **Medium:** `follow()` used a stale `lastLookaheadPoint` when no circle/segment
  intersection was found — replaced with forward-scanning closest-point fallback.
- **Low:** `getCurvature` used `tan(θ)` which overflows at θ=π/2 — rewrote using the
  full `sin/cos` line equation form, safe at all headings.
- **Low:** `moveToPose` compared `horizontalDrift == 0` with float equality — changed to `< 1e-6`.
- **CLI:** `vortex-cli` never bumped the version on a plain run — `apply_version()` now
  always runs before the release/dev branch split.

### Changed
- `Timer` rewritten with `startTime` snapshot; all getters are now idempotent.
- `getCurvature` rewritten without `tan()`.
- `follow()` lookahead selection uses monotonically advancing segment index.
- Brain screen telemetry improved: field map with robot dot + arrow, motor temps, battery %.
- Motion profile uses `g = 386 in/s²` (correct inches unit) in the slip-speed formula.
- `infoSink` and `bufferedStdout` merged into one file to reduce build complexity.

## [2.0.0] — 2025
### Added
- Arc-based odometry backend
- Pure Pursuit path following
- Curvature-based velocity profiling
- IMU drift compensation
- SD card CSV logging
- `vortex-cli` release automation

## [1.x]
- Initial LemLib-inspired odometry and PID motion control
