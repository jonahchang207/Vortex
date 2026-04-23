#pragma once
#include <cmath>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

namespace vortex {
struct Pose;

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

    /**
     * @brief Calculate the signed curvature of a circle tangent to a pose and passing through a point
     */
    double getCurvature(Pose pose, double x, double y);

    /**
     * @brief Exponential Moving Average filter
     */
    class EMA {
    public:
        EMA(double alpha) : alpha(alpha) {}
        double filter(double input) {
            output = alpha * input + (1.0 - alpha) * output;
            return output;
        }
    private:
        double alpha;
        double output = 0;
    };
} // namespace math
} // namespace vortex
