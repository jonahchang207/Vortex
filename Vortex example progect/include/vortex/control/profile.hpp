#pragma once
#include <cmath>
#include <algorithm>

namespace vortex {

/**
 * @brief Simple Trapezoidal Velocity Profile
 */
class TrapezoidalProfile {
public:
    /**
     * @brief Construct a new Trapezoidal Profile object
     * 
     * @param max_speed Maximum allowed speed (output units, e.g., 127)
     * @param accel Acceleration (output units per tick)
     * @param decel Deceleration (output units per tick)
     */
    TrapezoidalProfile(double max_speed, double accel, double decel)
        : max_speed(max_speed), accel(accel), decel(decel) {}

    /**
     * @brief Calculate the target speed for the current distance
     * 
     * @param current_dist Current distance to the target
     * @param current_speed Current speed (velocity)
     * @return double Target speed
     */
    double calculate(double current_dist, double current_speed) {
        // Calculate max speed we can have and still stop in time
        // v^2 = 2 * a * d  =>  v = sqrt(2 * a * d)
        double stopping_speed = std::sqrt(2.0 * decel * std::abs(current_dist));
        
        // Target speed is limited by max_speed and the stopping curve
        double target = std::min(max_speed, stopping_speed);
        
        // Acceleration limiting (Slew)
        if (current_speed < target) {
            target = std::min(target, current_speed + accel);
        } else if (current_speed > target) {
            target = std::max(target, current_speed - decel);
        }
        
        return target;
    }

private:
    double max_speed;
    double accel;
    double decel;
};

} // namespace vortex
