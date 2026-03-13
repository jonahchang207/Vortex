#pragma once
#include <cmath>
#include <algorithm>
#include <vector>
#include "Vortex/pose.hpp"

namespace vortex {

/** Which direction to turn. */
enum class AngularDirection {
    CW_CLOCKWISE,
    CCW_COUNTERCLOCKWISE,
    AUTO  ///< shortest path
};

/** Which drivetrain side to lock during a swing turn. */
enum class DriveSide { LEFT, RIGHT };

// ── Angle utilities ──────────────────────────────────────────────────────────

/** Normalize angle to [-π, π]. */
inline double normalizeAngle(double angle) {
    return std::remainder(angle, 2 * M_PI);
}

/** Normalize angle to [0, 2π] (radians) or [0, 360] (degrees). */
constexpr double sanitizeAngle(double angle, bool radians = true) {
    double full = radians ? 2 * M_PI : 360.0;
    return std::fmod(std::fmod(angle, full) + full, full);
}

/** Signed angular error from position to target, respecting direction preference. */
double angleError(double target, double position, bool radians = true,
                  AngularDirection direction = AngularDirection::AUTO);

/** Degrees → Radians. */
constexpr double degToRad(double deg) { return deg * M_PI / 180.0; }

/** Radians → Degrees. */
constexpr double radToDeg(double rad) { return rad * 180.0 / M_PI; }

// ── Math helpers ─────────────────────────────────────────────────────────────

/** Slew-rate limiter: limits how fast value can change per step. */
double slew(double target, double current, double maxStep);

/** Returns 0 if |value| < threshold, otherwise returns value unchanged. */
inline double deadband(double value, double threshold) {
    return (std::abs(value) < threshold) ? 0.0 : value;
}

/** Sign function: returns +1 or -1. */
template <typename T> constexpr int sgn(T v) { return (v < 0) ? -1 : 1; }

/** Average of a vector. Returns 0 for empty input. */
double avg(std::vector<double> values);

/** Exponential moving average: smooth=1 → fully current, smooth=0 → fully previous. */
double ema(double current, double previous, double smooth);

// ── Geometry ─────────────────────────────────────────────────────────────────

/**
 * @brief Signed curvature of the circle passing through `pose` and `other`,
 *        tangent to the robot's heading at `pose`.
 *
 *        Positive = turn left (CCW), Negative = turn right (CW).
 *        Safe at all headings — avoids tan(θ) overflow via explicit line equation.
 */
double getCurvature(Pose pose, Pose other);

/** Intersection points of a line segment (p1→p2) with a circle (center, radius). */
std::vector<Pose> lineCircleIntersection(Pose p1, Pose p2, Pose center, double radius);

// ── Motion profile ───────────────────────────────────────────────────────────

/**
 * @brief Trapezoidal velocity profile.
 *
 * Returns the target speed at position `dist` along a move of total length `totalDist`.
 * Accelerates from rest, then decelerates to rest at the end.
 *
 * @param dist       Distance already traveled (inches).
 * @param totalDist  Total planned distance (inches).
 * @param maxVel     Peak velocity (motor units 0–127).
 * @param maxAccel   Max acceleration (velocity units per inch).
 * @param currentVel Current velocity (for continuity).
 */
double motionProfile(double dist, double totalDist, double maxVel,
                     double maxAccel, double currentVel);

} // namespace vortex
