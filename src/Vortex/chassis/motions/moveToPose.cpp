#include "Vortex/chassis/chassis.hpp"
#include "Vortex/util.hpp"
#include "Vortex/timer.hpp"
#include "pros/rtos.hpp"
#include <cmath>

namespace vortex {

void Chassis::moveToPose(float x, float y, float theta, int timeout,
                         MoveToPoseParams params, bool async) {
    if (async) {
        pros::Task([this, x, y, theta, timeout, params]() {
            moveToPose(x, y, theta, timeout, params, false);
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
    angularLargeExit.reset();
    angularSmallExit.reset();

    // Convert target heading to our internal CW-from-north radian convention.
    // If going in reverse, flip the heading 180° so the robot backs into the pose.
    Pose target(x, y, M_PI_2 - degToRad(theta));
    if (!params.forwards) target.theta = std::fmod(target.theta + M_PI, 2 * M_PI);

    // Fall back to drivetrain's horizontalDrift if caller didn't override it
    if (std::abs(params.horizontalDrift) < 1e-6)
        params.horizontalDrift = drivetrain.horizontalDrift;

    Pose   lastPose       = getPose();
    double prevLatOut     = 0;
    double prevAngOut     = 0;
    bool   close          = false;
    bool   lateralSettled = false;
    bool   prevSameSide   = false;
    distTraveled          = 0;
    Timer  timer(timeout);

    while (!timer.isDone() &&
           ((!lateralSettled || (!angularLargeExit.getExit() && !angularSmallExit.getExit())) || !close) &&
           this->motionRunning)
    {
        Pose pose = getPose(true, true);
        distTraveled += pose.distance(lastPose);
        lastPose = pose;

        double distToTarget = pose.distance(target);

        // Switch to close mode within 7.5 inches
        if (distToTarget < 7.5 && !close) {
            close = true;
            params.maxSpeed = std::max(std::abs(prevLatOut), 60.0f);
        }

        if (lateralLargeExit.getExit() && lateralSmallExit.getExit())
            lateralSettled = true;

        // Carrot point: a point along the final approach vector, `lead * dist` behind target.
        // This creates a smooth curved approach rather than a sharp two-step turn-then-move.
        Pose carrot = target - Pose(std::cos(target.theta), std::sin(target.theta))
                             * params.lead * distToTarget;
        if (close) carrot = target; // snap to actual target when close

        // Side check: detect when the robot has passed the target's approach plane.
        // If the robot and carrot are on opposite sides, and we're close with minSpeed,
        // exit early to avoid oscillating back and forth across the target line.
        bool robotSide  = (pose.y  - target.y) * -std::sin(target.theta)
                       <= (pose.x  - target.x) *  std::cos(target.theta) + params.earlyExitRange;
        bool carrotSide = (carrot.y - target.y) * -std::sin(target.theta)
                       <= (carrot.x - target.x) *  std::cos(target.theta) + params.earlyExitRange;
        bool sameSide = (robotSide == carrotSide);
        if (!sameSide && prevSameSide && close && params.minSpeed != 0) break;
        prevSameSide = sameSide;

        // Angular error: angle between robot heading and direction to carrot (or final target)
        double robotTheta = params.forwards ? pose.theta : pose.theta + M_PI;
        double angularError = close
            ? angleError(robotTheta, target.theta)
            : angleError(robotTheta, pose.angle(carrot));

        // Lateral error: distance to carrot projected onto heading
        double lateralError;
        if (close) {
            lateralError = pose.distance(carrot) * std::cos(angleError(pose.theta, pose.angle(carrot)));
        } else {
            lateralError = pose.distance(carrot) * sgn(std::cos(angleError(pose.theta, pose.angle(carrot))));
        }

        lateralSmallExit.update(lateralError);
        lateralLargeExit.update(lateralError);
        angularSmallExit.update(radToDeg(angularError));
        angularLargeExit.update(radToDeg(angularError));

        double lateralOut = lateralPID.update(lateralError);
        double angularOut = angularPID.update(radToDeg(angularError));

        // Clamp and slew
        angularOut = std::clamp(angularOut, (double)-params.maxSpeed, (double)params.maxSpeed);
        lateralOut = std::clamp(lateralOut, (double)-params.maxSpeed, (double)params.maxSpeed);
        if (!close) lateralOut = slew(lateralOut, prevLatOut, linearSettings.slew);

        // Curvature-based lateral speed cap: slow down on tight curves to prevent
        // the robot from sliding wide. Uses centripetal friction model:
        //   v_max = sqrt(mu * r * g)   where r = 1/|curvature|, g=386 in/s²
        double curvature = getCurvature(pose, carrot);
        if (std::abs(curvature) > 1e-6) {
            double radius    = 1.0 / std::abs(curvature);
            double slipSpeed = std::sqrt(params.horizontalDrift * radius * 386.0);
            lateralOut = std::clamp(lateralOut, -slipSpeed, slipSpeed);
        }

        // Prioritize turning: reduce lateral if combined output exceeds maxSpeed
        double overturn = std::abs(angularOut) + std::abs(lateralOut) - params.maxSpeed;
        if (overturn > 0) lateralOut -= sgn(lateralOut) * overturn;

        // Direction enforcement
        if ( params.forwards && !close) lateralOut = std::max(lateralOut, 0.0);
        if (!params.forwards && !close) lateralOut = std::min(lateralOut, 0.0);

        // Minimum speed
        if ( params.forwards && lateralOut > 0 && lateralOut <  std::abs(params.minSpeed))
            lateralOut =  std::abs(params.minSpeed);
        if (!params.forwards && lateralOut < 0 && lateralOut > -std::abs(params.minSpeed))
            lateralOut = -std::abs(params.minSpeed);

        prevLatOut = lateralOut;
        prevAngOut = angularOut;

        double leftPow  = lateralOut + angularOut;
        double rightPow = lateralOut - angularOut;
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
