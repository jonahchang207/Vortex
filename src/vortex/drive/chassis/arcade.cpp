#include "vortex/drive/chassis.hpp"
#include <algorithm>

namespace vortex {

void Chassis::arcade(int forward, int turn, double curve) {
    // Steering Desaturation Logic
    // Prioritize turn over forward speed when motors saturate
    int left = forward + turn;
    int right = forward - turn;

    int max_val = std::max(std::abs(left), std::abs(right));
    if (max_val > 127) {
        // Bias towards turning (desaturate_bias = 1.0 means full turn priority)
        double scale = (127.0 - (desaturate_bias * std::abs(turn))) / (max_val - (desaturate_bias * std::abs(turn)));
        left = turn + (forward * scale);
        right = -turn + (forward * scale);
    }

    config.left_motors->move(left);
    config.right_motors->move(right);
}

void Chassis::arcadeSplit(int forward, int turn, double curve) {
    // Split arcade uses the same mixing as standard arcade
    arcade(forward, turn, curve);
}

} // namespace vortex
