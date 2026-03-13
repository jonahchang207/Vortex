#include "Vortex/chassis/chassis.hpp"
#include "Vortex/util.hpp"
#include "Vortex/timer.hpp"
#include "pros/rtos.hpp"
#include <cmath>

namespace vortex {

void Chassis::turnToPoint(float x, float y, int timeout,
                          TurnToPointParams params, bool async) {
    if (async) {
        pros::Task([this, x, y, timeout, params]() {
            turnToPoint(x, y, timeout, params, false);
        });
        pros::delay(10);
        return;
    }

    this->requestMotionStart();
    if (!this->motionRunning) return;

    angularPID.reset();
    angularLargeExit.reset();
    angularSmallExit.reset();

    const Pose target(x, y);
    Pose   lastPose  = getPose();
    double prevPower = 0;
    distTraveled     = 0;
    Timer  timer(timeout);

    while (!timer.isDone() &&
           !angularLargeExit.getExit() && !angularSmallExit.getExit() &&
           this->motionRunning)
    {
        Pose pose = getPose(true, true); // radians, standardPos
        distTraveled += pose.distance(lastPose);
        lastPose = pose;

        // Bearing to target in standard-position radians, then convert to degrees.
        // pose.angle() uses atan2(east, north) which matches our CW-from-north convention.
        double bearingRad   = pose.angle(target);
        double robotTheta   = params.forwards ? pose.theta : pose.theta + M_PI;

        // angleError with radians=true → result in radians → convert once to degrees.
        // Bug that was here: angleError was called with radians=false (returns degrees),
        // then the result was wrapped in radToDeg() again — double conversion.
        double error = radToDeg(angleError(robotTheta, bearingRad, true, params.direction));

        double power = angularPID.update(error);
        angularLargeExit.update(error);
        angularSmallExit.update(error);

        power = std::clamp(power, (double)-params.maxSpeed, (double)params.maxSpeed);
        power = slew(power, prevPower, angularSettings.slew);

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
