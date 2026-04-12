#include "vortex/util/math.hpp"
#include <cmath>
#include <algorithm>

namespace vortex {
namespace math {

double wrapAngle(double angle) {
    while (angle <= -PI) angle += 2 * PI;
    while (angle > PI) angle -= 2 * PI;
    return angle;
}

double wrapAngleDeg(double angleDeg) {
    while (angleDeg <= -180) angleDeg += 360;
    while (angleDeg > 180) angleDeg -= 360;
    return angleDeg;
}

double dist(double x1, double y1, double x2, double y2) {
    return std::hypot(x2 - x1, y2 - y1);
}

double getCurvature(Pose pose, double x, double y) {
    // Formula for curvature of circle tangent to pose and passing through (x, y)
    // side = sign of (sin(theta) * (x - x0) - cos(theta) * (y - y0))
    // r = (dist^2) / (2 * |side|)
    // curvature = 1/r
    double theta = pose.theta * DEG_TO_RAD;
    double dx = x - pose.x;
    double dy = y - pose.y;
    double side = std::sin(theta) * dx - std::cos(theta) * dy;
    
    if (side == 0) return 0;
    double distance = std::hypot(dx, dy);
    return (2.0 * side) / (distance * distance);
}

} // namespace math
} // namespace vortex
