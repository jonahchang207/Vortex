#include "vortex/drive/chassis.hpp"
#include "vortex/util/math.hpp"
#include "vortex/util/timer.hpp"

namespace vortex {

void Chassis::swingToHeading(double theta, DriveSide side, int timeout, SwingParams params) {
    if (params.async) return;

    target_x = 99999;
    target_y = 99999;

    Timer timer;
    angular_pid.reset();

    TrapezoidalProfile profile(params.max_speed, params.accel, params.decel);
    double last_speed = 0;

    while (timer.getElapsed() < (uint32_t)timeout) {
        Pose pose = odom.getPose();
        double error = math::wrapAngleDeg(theta - pose.theta);
        
        if (params.direction == AngularDirection::CW_CLOCKWISE && error < 0) error += 360;
        if (params.direction == AngularDirection::CCW_COUNTERCLOCKWISE && error > 0) error -= 360;

        angle_to_target = error;
        double power = angular_pid.update(error);
        double profile_speed = profile.calculate(std::abs(error), last_speed);

        if (std::abs(power) > profile_speed) power = std::copysign(profile_speed, power);
        if (std::abs(power) < params.min_speed) power = std::copysign(params.min_speed, power);

        last_speed = std::abs(power);
        
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
