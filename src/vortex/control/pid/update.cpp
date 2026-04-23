#include "vortex/control/pid.hpp"
#include <cmath>

namespace vortex {

double PID::update(double error) {
    double derivative = error - prev_error;
    integral += error;

    // Integral Anti-Windup
    if (settings.windup_range > 0 && std::abs(error) > settings.windup_range) integral = 0;

    double output = (error * settings.kP) + (integral * settings.kI) + (derivative * settings.kD);
    
    // Slew Rate
    if (settings.slew > 0) {
        double delta = output - prev_output;
        if (std::abs(delta) > settings.slew) {
            output = prev_output + (delta > 0 ? settings.slew : -settings.slew);
        }
    }

    prev_error = error;
    prev_output = output;
    return output;
}

} // namespace vortex
