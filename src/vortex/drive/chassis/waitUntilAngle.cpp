#include "vortex/drive/chassis.hpp"
#include "vortex/util/math.hpp"
#include <cmath>

namespace vortex {

void Chassis::waitUntilAngle(double degrees) {
    while (std::abs(angular_pid.getError()) > degrees) {
        pros::delay(10);
    }
}

} // namespace vortex
