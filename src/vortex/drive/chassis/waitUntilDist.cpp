#include "vortex/drive/chassis.hpp"
#include <cmath>

namespace vortex {

void Chassis::waitUntilDist(double inches) {
    double start_dist = odom.getDistanceTraveled();
    while (std::abs(odom.getDistanceTraveled() - start_dist) < inches) {
        pros::delay(10);
    }
}

} // namespace vortex
