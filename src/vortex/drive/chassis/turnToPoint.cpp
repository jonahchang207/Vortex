#include "vortex/drive/chassis.hpp"
#include "vortex/util/math.hpp"
#include <cmath>

namespace vortex {

void Chassis::turnToPoint(double x, double y, int timeout, TurnParams params) {
    Pose pose = odom.getPose();
    double angle = std::atan2(y - pose.y, x - pose.x) * 180.0 / M_PI;
    turnToHeading(angle, timeout, params);
}

} // namespace vortex
