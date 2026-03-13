#pragma once
#include <cmath>
#include <string>

namespace vortex {

/**
 * @brief A 2D pose: (x, y) position and heading theta.
 *
 * Convention used throughout Vortex:
 *   - +X is East (right on the field)
 *   - +Y is North (forward on the field at theta=0)
 *   - Theta increases clockwise (like a compass bearing)
 *   - Theta = 0 faces North (+Y), Theta = π/2 faces East (+X)
 */
class Pose {
public:
    double x;
    double y;
    double theta;

    Pose(double x = 0, double y = 0, double theta = 0) : x(x), y(y), theta(theta) {}

    Pose operator+(const Pose& o) const { return {x + o.x, y + o.y, theta}; }
    Pose operator-(const Pose& o) const { return {x - o.x, y - o.y, theta}; }
    Pose operator*(double s)      const { return {x * s,   y * s,   theta}; }
    Pose operator/(double s)      const { return {x / s,   y / s,   theta}; }

    /** Dot product of the (x, y) components — heading is ignored. */
    double dot(const Pose& o) const { return x * o.x + y * o.y; }

    /** Linear interpolation between this pose and another (t in [0,1]). */
    Pose lerp(const Pose& o, double t) const {
        return {x + (o.x - x) * t, y + (o.y - y) * t, theta};
    }

    /** Euclidean distance between the (x, y) components of two poses. */
    double distance(const Pose& o) const {
        return std::hypot(o.x - x, o.y - y);
    }

    /**
     * @brief Angle (radians) from this pose's position to another pose's position.
     * Returns a value in (-π, π] in the global frame.
     */
    double angle(const Pose& o) const {
        return std::atan2(o.x - x, o.y - y); // atan2(east, north) — matches CW-from-north
    }

    /** Rotate the (x, y) vector by an angle (radians). Heading is unchanged. */
    Pose rotate(double a) const {
        return {x * std::cos(a) - y * std::sin(a),
                x * std::sin(a) + y * std::cos(a), theta};
    }
};

} // namespace vortex
