#include "vortex/drive/chassis.hpp"
#include "vortex/util/math.hpp"
#include <cmath>

namespace vortex {

void Chassis::swingToPoint(double x, double y, DriveSide side, int timeout, SwingParams params) {
    Pose pose = odom.getPose();
    double angle = pose.angleTo(Pose(x, y));
    swingToHeading(angle, side, timeout, params);
}

} // namespace vortex
