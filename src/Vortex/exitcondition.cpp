#include "Vortex/exitcondition.hpp"
#include "pros/rtos.hpp"
#include <cmath>

namespace vortex {

ExitCondition::ExitCondition(double range, int settleTime, int maxTime)
    : range(range), settleTime(settleTime), maxTime(maxTime) {}

void ExitCondition::reset() {
    startTime   = 0;
    firstInTime = 0;
}

void ExitCondition::update(double error) {
    if (startTime == 0) startTime = pros::millis(); // latch start on first update

    if (std::abs(error) > range) {
        firstInTime = 0; // left the settled zone — restart settle timer
    } else if (firstInTime == 0) {
        firstInTime = pros::millis(); // just entered the settled zone
    }
}

bool ExitCondition::getExit() const {
    if (startTime == 0) return false;

    // Hard timeout: abort if motion has taken too long overall
    if (maxTime > 0 && (int)(pros::millis() - startTime) > maxTime) return true;

    // Settle: error stayed inside range for long enough
    if (firstInTime != 0 && (int)(pros::millis() - firstInTime) > settleTime) return true;

    return false;
}

} // namespace vortex
