#include "vortex/drive/chassis.hpp"
#include <cmath>

namespace vortex {

void Chassis::tank(int left, int right, double curve) {
    if (curve != 0.0) {
        // Apply exponential curve logic (simplified here)
        auto applyCurve = [&](double input) {
            double sign = (input > 0) ? 1.0 : -1.0;
            return sign * std::pow(std::abs(input) / 127.0, curve) * 127.0;
        };
        left = applyCurve(left);
        right = applyCurve(right);
    }
    config.left_motors->move(left);
    config.right_motors->move(right);
}

} // namespace vortex
