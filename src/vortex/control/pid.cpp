#include "vortex/control/pid.hpp"

namespace vortex {

PID::PID(double kP, double kI, double kD) 
    : kP(kP), kI(kI), kD(kD) {}

PID::PID(PIDConstants constants)
    : kP(constants.kP), kI(constants.kI), kD(constants.kD) {}

double PID::update(double error) {
    integral += error;
    double derivative = error - prev_error;
    prev_error = error;

    return (error * kP) + (integral * kI) + (derivative * kD);
}

void PID::reset() {
    integral = 0;
    prev_error = 0;
}

void PID::setConstants(PIDConstants constants) {
    kP = constants.kP;
    kI = constants.kI;
    kD = constants.kD;
}

} // namespace vortex
