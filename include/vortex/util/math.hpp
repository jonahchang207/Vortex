#pragma once
#include <cmath>

namespace vortex {
namespace math {
    // Math constants
    constexpr double PI = M_PI;
    constexpr double DEG_TO_RAD = PI / 180.0;
    constexpr double RAD_TO_DEG = 180.0 / PI;

    /**
     * @brief Wraps an angle in radians to [-PI, PI)
     * @param angle The angle to wrap
     * @return The wrapped angle
     */
    double wrapAngle(double angle);
    
    /**
     * @brief Wraps an angle in degrees to [-180, 180)
     * @param angleDeg The angle to wrap
     * @return The wrapped angle
     */
    double wrapAngleDeg(double angleDeg);

    /**
     * @brief Calculates Euclidean distance between two points
     */
    double dist(double x1, double y1, double x2, double y2);
} // namespace math
} // namespace vortex
