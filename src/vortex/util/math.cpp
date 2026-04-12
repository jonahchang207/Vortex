#include "vortex/util/math.hpp"

namespace vortex {
namespace math {

double wrapAngle(double angle) {
    while (angle >= PI) angle -= 2 * PI;
    while (angle < -PI) angle += 2 * PI;
    return angle;
}

double wrapAngleDeg(double angleDeg) {
    while (angleDeg >= 180) angleDeg -= 360;
    while (angleDeg < -180) angleDeg += 360;
    return angleDeg;
}

double dist(double x1, double y1, double x2, double y2) {
    return std::hypot(x2 - x1, y2 - y1);
}

} // namespace math
} // namespace vortex
