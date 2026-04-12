#include "vortex/util/math.hpp"
#include <cmath>

namespace vortex {

double wrapAngle(double rad) {
    while (rad > M_PI) rad -= 2 * M_PI;
    while (rad < -M_PI) rad += 2 * M_PI;
    return rad;
}

double wrapAngleDeg(double deg) {
    while (deg > 180.0) deg -= 360.0;
    while (deg < -180.0) deg += 360.0;
    return deg;
}

} // namespace vortex
