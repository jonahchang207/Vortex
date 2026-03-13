#include "Vortex/chassis/chassis.hpp"
#include "Vortex/util.hpp"
#include "Vortex/timer.hpp"
#include "pros/rtos.hpp"
#include <cmath>
#include <optional>

namespace vortex {

void Chassis::turnToHeading(float theta, int timeout,
                            TurnToHeadingParams params, bool async) {
    if (async) {
        pros::Task([this, theta, timeout, params]() {
            turnToHeading(theta, timeout, params, false);
        });
        pros::delay(10);
        return;
    }

    this->requestMotionStart();
    if (!this->motionRunning) return;

    angularPID.reset();
    angularLargeExit.reset();
    angularSmallExit.reset();

    double startTheta  = getPose().theta; // degrees
    distTraveled       = 0;
    double prevPower   = 0;
    bool   settling    = false;
    std::optional<double> prevError;
    Timer timer(timeout);

    while (!timer.isDone() &&
           !angularLargeExit.getExit() && !angularSmallExit.getExit() &&
           this->motionRunning)
    {
        double currentTheta = getPose().theta; // degrees
        distTraveled = std::abs(angleError(currentTheta, startTheta, false));

        // Once the error crosses zero (overshoots), lock to the shortest-path
        // direction regardless of the original direction param to let it settle.
        double rawError = angleError(theta, currentTheta, false);
        if (prevError && sgn(rawError) != sgn(*prevError)) settling = true;
        prevError = rawError;

        double error = settling
            ? angleError(theta, currentTheta, false)
            : angleError(theta, currentTheta, false, params.direction);

        double power = angularPID.update(error);
        angularLargeExit.update(error);
        angularSmallExit.update(error);

        power = std::clamp(power, (double)-params.maxSpeed, (double)params.maxSpeed);
        if (std::abs(error) > 20) power = slew(power, prevPower, angularSettings.slew);

        if (power < 0 && power > -(double)params.minSpeed) power = -(double)params.minSpeed;
        if (power > 0 && power <  (double)params.minSpeed) power =  (double)params.minSpeed;

        prevPower = power;
        drivetrain.leftMotors->move(power);
        drivetrain.rightMotors->move(-power);
        pros::delay(10);
    }

    drivetrain.leftMotors->move(0);
    drivetrain.rightMotors->move(0);
    distTraveled = -1;
    this->endMotion();
}

} // namespace vortex
