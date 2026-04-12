#include "vortex/util/pose.hpp"
#include "vortex/util/math.hpp"

namespace vortex {

double Pose::distance(const Pose& other) const {
    return math::dist(x, y, other.x, other.y);
}

double Pose::angleTo(const Pose& other) const {
    // Calculates absolute angle to another point
    // Note: uses 0 degrees as positive Y (forward) convention in odometry
    return std::atan2(other.x - x, other.y - y) * math::RAD_TO_DEG;
}

} // namespace vortex
