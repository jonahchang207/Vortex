#include "Vortex/chassis/chassis.hpp"
#include "pros/rtos.hpp"

namespace vortex {

void Chassis::turnThenMove(float x, float y, int timeout,
                           MoveToPointParams params, bool async) {
    if (async) {
        pros::Task([this, x, y, timeout, params]() {
            turnThenMove(x, y, timeout, params, false);
        });
        pros::delay(10);
        return;
    }

    TurnToPointParams turnParams;
    turnParams.forwards      = params.forwards;
    turnParams.maxSpeed      = (int)params.maxSpeed;
    turnParams.minSpeed      = (int)params.minSpeed;
    turnParams.earlyExitRange = params.earlyExitRange;

    turnToPoint(x, y, timeout, turnParams, false);
    moveToPoint(x, y, timeout, params,     false);
}

} // namespace vortex
