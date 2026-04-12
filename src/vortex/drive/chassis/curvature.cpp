#include "vortex/drive/chassis.hpp"
#include <cmath>
#include <algorithm>

namespace vortex {

void Chassis::curvature(int throttle, int turn, double curve) {
    if (throttle == 0) {
        // Fallback to regular arcade for point turns
        arcade(0, turn, curve);
        return;
    }

    double left, right;
    double abs_throttle = std::abs(throttle);
    
    // Curvature logic: turn scales the radius of the arc
    // Radius = 1 / curvature
    left = throttle + (abs_throttle * (turn / 127.0));
    right = throttle - (abs_throttle * (turn / 127.0));

    // Normalize to max motor power
    double max_mag = std::max(std::abs(left), std::abs(right));
    if (max_mag > 127.0) {
        left = (left / max_mag) * 127.0;
        right = (right / max_mag) * 127.0;
    }

    config.left_motors->move(left);
    config.right_motors->move(right);
}

} // namespace vortex
