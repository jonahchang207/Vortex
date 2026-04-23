#include "vortex/drive/chassis.hpp"
#include "vortex/util/math.hpp"
#include "vortex/util/timer.hpp"

namespace vortex {

void Chassis::swingToHeading(double theta, DriveSide side, int timeout, SwingParams params) {
    if (params.async) return;

    Timer timer;
    angular_pid.reset();

    while (timer.getElapsed() < (uint32_t)timeout) {
        Pose pose = odom.getPose();
        double error = math::wrapAngleDeg(theta - pose.theta);
        
        if (params.direction == AngularDirection::CW_CLOCKWISE && error < 0) error += 360;
        if (params.direction == AngularDirection::CCW_COUNTERCLOCKWISE && error > 0) error -= 360;

        double power = angular_pid.update(error);
        
        if (angular_pid.isSettled(error)) break;

        if (side == DriveSide::LEFT) {
            config.left_motors->move(0);
            config.right_motors->move(-power); // Turning right moves right motors back
        } else {
            config.left_motors->move(power);
            config.right_motors->move(0);
        }
        pros::delay(10);
    }
    arcade(0, 0);
}

} // namespace vortex
