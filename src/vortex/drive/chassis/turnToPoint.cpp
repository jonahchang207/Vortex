#include "vortex/drive/chassis.hpp"
#include "vortex/util/math.hpp"
#include <cmath>

namespace vortex {

void Chassis::turnToPoint(double x, double y, int timeout, TurnParams params) {
    Pose pose = odom.getPose();
    double angle = pose.angleTo(Pose(x, y));
    turnToHeading(angle, timeout, params);
}

} // namespace vortex
