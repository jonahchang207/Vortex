#include "vortex/control/pid.hpp"
#include <cmath>

namespace vortex {

bool PID::isSettled(double error) {
    return exit.update(error);
}

void PID::reset() {
    prev_error = 0;
    prev_output = 0;
    integral = 0;
    exit.reset();
}

} // namespace vortex
