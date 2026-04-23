#include "vortex/control/pid.hpp"

namespace vortex {

PID::PID(double kP, double kI, double kD) 
    : settings({kP, kI, kD}), exit({0, 0, 0}) {}

PID::PID(PIDSettings settings, ExitCondition exit_condition) 
    : settings(settings), exit(exit_condition) {}

void PID::setSettings(PIDSettings settings) {
    this->settings = settings;
}

void PID::setExitCondition(ExitCondition exit_condition) {
    this->exit = exit_condition;
}

} // namespace vortex
