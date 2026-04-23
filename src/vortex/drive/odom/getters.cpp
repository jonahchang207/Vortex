#include "vortex/drive/odom.hpp"

namespace vortex {

Pose Odom::getPose(bool radians) {
    Pose p = pose;
    if (radians) p.theta = p.theta * 3.141592653589793 / 180.0;
    return p;
}

double Odom::getDistanceTraveled() {
    return total_dist;
}

void Odom::resetDistanceTraveled() {
    total_dist = 0;
}

} // namespace vortex
