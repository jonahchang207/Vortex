#include "Vortex/chassis/chassis.hpp"
#include "Vortex/util.hpp"
#include "Vortex/timer.hpp"
#include "pros/rtos.hpp"
#include <cmath>

namespace vortex {

void Chassis::moveToPoint(float x, float y, int timeout,
                          MoveToPointParams params, bool async) {
    if (async) {
        pros::Task([this, x, y, timeout, params]() {
            moveToPoint(x, y, timeout, params, false);
        });
        pros::delay(10);
        return;
    }

    this->requestMotionStart();
    if (!this->motionRunning) return;

    lateralPID.reset();
    lateralLargeExit.reset();
    lateralSmallExit.reset();
    angularPID.reset();

    const Pose target(x, y);

    // Capture total planned distance BEFORE the loop.
    // Recalculating inside the loop would make the motion profiler think the
    // total distance changes every iteration as the robot curves, producing
    // erratic velocity targets.
    const double totalDist = std::max(getPose(true, true).distance(target), 0.01);

    Pose   lastPose   = getPose();
    double prevLatOut = 0;
    double prevAngOut = 0;
    bool   close      = false;
    distTraveled      = 0;
    Timer  timer(timeout);

    while (!timer.isDone() &&
           ((!lateralSmallExit.getExit() && !lateralLargeExit.getExit()) || !close) &&
           this->motionRunning)
    {
        Pose pose = getPose(true, true);
        distTraveled += pose.distance(lastPose);
        lastPose = pose;

        // Enter "close" mode within 7.5 inches: disable angular correction and
        // cap speed to prevent overshoot past the target point.
        if (pose.distance(target) < 7.5 && !close) {
            close = true;
            params.maxSpeed = std::max(std::abs(prevLatOut), 60.0f);
        }

        // Bearing from robot to target
        double bearing        = pose.angle(target);
        double robotTheta     = params.forwards ? pose.theta : pose.theta + M_PI;
        double angularError   = angleError(robotTheta, bearing); // radians

        // Lateral error: signed distance to target projected onto forward axis.
        // When robot is badly mis-aimed, the cosine term reduces forward drive
        // so the angular correction can steer first.
        double distToTarget = pose.distance(target);
        double lateralError = distToTarget * std::cos(angleError(pose.theta, bearing));

        lateralSmallExit.update(lateralError);
        lateralLargeExit.update(lateralError);

        // Trapezoidal profile gives a velocity ceiling based on position in the move.
        // PID handles fine-grained error correction within that ceiling.
        double profileCap = motionProfile(distTraveled, totalDist,
                                          linearSettings.maxVelocity,
                                          linearSettings.maxAccel, prevLatOut);
        double lateralOut = lateralPID.update(lateralError);
        if (std::abs(lateralOut) > profileCap) lateralOut = sgn(lateralOut) * profileCap;

        double angularOut = close ? 0.0 : angularPID.update(radToDeg(angularError));

        // Slew and clamp
        angularOut = slew(std::clamp(angularOut, (double)-params.maxSpeed, (double)params.maxSpeed),
                          prevAngOut, angularSettings.slew);
        lateralOut = std::clamp(lateralOut, (double)-params.maxSpeed, (double)params.maxSpeed);
        if (!close) lateralOut = slew(lateralOut, prevLatOut, linearSettings.slew);

        // Enforce direction
        if ( params.forwards && !close) lateralOut = std::max(lateralOut, 0.0);
        if (!params.forwards && !close) lateralOut = std::min(lateralOut, 0.0);

        // Minimum speed (overcomes static friction)
        if (params.forwards  && lateralOut > 0 && lateralOut <  std::abs(params.minSpeed))
            lateralOut =  std::abs(params.minSpeed);
        if (!params.forwards && lateralOut < 0 && lateralOut > -std::abs(params.minSpeed))
            lateralOut = -std::abs(params.minSpeed);

        prevLatOut = lateralOut;
        prevAngOut = angularOut;

        double leftPow  = lateralOut + angularOut;
        double rightPow = lateralOut - angularOut;

        // Desaturate: scale both sides down if either exceeds maxSpeed
        double ratio = std::max(std::abs(leftPow), std::abs(rightPow)) / params.maxSpeed;
        if (ratio > 1.0) { leftPow /= ratio; rightPow /= ratio; }

        drivetrain.leftMotors->move(leftPow);
        drivetrain.rightMotors->move(rightPow);
        pros::delay(10);
    }

    drivetrain.leftMotors->move(0);
    drivetrain.rightMotors->move(0);
    distTraveled = -1;
    this->endMotion();
}

} // namespace vortex
