#include "Vortex/pid.hpp"
#include <cmath>

namespace vortex {

PID::PID(double kP, double kI, double kD, double windupRange, bool signFlipReset)
    : constants({kP, kI, kD, windupRange, signFlipReset}) {}

PID::PID(Constants c) : constants(c) {}

double PID::update(double error) {
    // Accumulate integral only when inside the windup range (or range is disabled)
    if (constants.windupRange == 0 || std::abs(error) < constants.windupRange)
        integral += error;
    else
        integral = 0;

    // Sign-flip reset: clear integral when error crosses zero to avoid overshoot
    if (constants.signFlipReset && std::signbit(error) != std::signbit(prevError))
        integral = 0;

    double derivative = error - prevError;
    prevError = error;

    return constants.kP * error + constants.kI * integral + constants.kD * derivative;
}

void PID::reset() {
    integral  = 0;
    prevError = 0;
}

} // namespace vortex
