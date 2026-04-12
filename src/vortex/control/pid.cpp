#include "vortex/control/pid.hpp"
#include <cmath>

namespace vortex {

PID::PID(double kP, double kI, double kD) 
    : settings({kP, kI, kD}), exit(0, 0, 0) {}

PID::PID(PIDSettings settings, ExitCondition exit_condition)
    : settings(settings), exit(exit_condition) {}

double PID::update(double error) {
    // Integral with anti-windup
    if (settings.windup_range == 0 || std::abs(error) < settings.windup_range) {
        integral += error;
    } else {
        integral = 0;
    }

    double derivative = error - prev_error;
    prev_error = error;

    double output = (error * settings.kP) + (integral * settings.kI) + (derivative * settings.kD);

    // Slew rate limiting
    if (settings.slew > 0) {
        double diff = output - prev_output;
        if (std::abs(diff) > settings.slew) {
            output = prev_output + (diff > 0 ? settings.slew : -settings.slew);
        }
    }

    prev_output = output;
    return output;
}

void PID::reset() {
    integral = 0;
    prev_error = 0;
    prev_output = 0;
    exit.reset();
}

bool PID::isSettled(double error) {
    return exit.update(error);
}

void PID::setSettings(PIDSettings settings) {
    this->settings = settings;
}

void PID::setExitCondition(ExitCondition exit_condition) {
    this->exit = exit_condition;
}

} // namespace vortex
