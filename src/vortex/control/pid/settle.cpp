#include "vortex/control/pid.hpp"
#include <cmath>

namespace vortex {

bool PID::isSettled(double error) {
    if (std::abs(error) < exit_condition.error_threshold) {
        if (settle_timer.getTime() > exit_condition.time_threshold) {
            return true;
        }
    } else {
        settle_timer.reset();
    }
    return false;
}

void PID::reset() {
    prev_error = 0;
    prev_output = 0;
    integral = 0;
    settle_timer.reset();
}

} // namespace vortex
