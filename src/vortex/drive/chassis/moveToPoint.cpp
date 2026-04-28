#include "vortex/drive/chassis.hpp"
#include "vortex/util/math.hpp"
#include "vortex/util/timer.hpp"
#include <cmath>

namespace vortex {

void Chassis::moveToPoint(double x, double y, int timeout, MoveToPointParams params) {
    if (params.async) {
        // Queue movement (future task system)
        return;
    }

    Timer timer;
    linear_pid.reset();
    angular_pid.reset();

    target_x = x;
    target_y = y;

    TrapezoidalProfile profile(params.max_speed, params.accel, params.decel);
    double last_speed = 0;

    while (timer.getElapsed() < (uint32_t)timeout) {
        Pose pose = odom.getPose();
        double distance = math::dist(pose.x, pose.y, x, y);
        double target_angle = pose.angleTo(Pose(x, y));

        if (!params.forwards) target_angle = math::wrapAngleDeg(target_angle + 180);
        double angle_error = math::wrapAngleDeg(target_angle - pose.theta);

        dist_to_target = distance;
        angle_to_target = angle_error;

        double linear_power = linear_pid.update(distance);
        double profile_speed = profile.calculate(distance, last_speed);

        // Clamp PID output to profile
        if (linear_power > profile_speed) linear_power = profile_speed;
        
        // Ensure min_speed for chaining
        if (linear_power < params.min_speed) linear_power = params.min_speed;
        
        last_speed = linear_power;

        double angular_power = angular_pid.update(angle_error);

        // Exit conditions
        if (linear_pid.isSettled(distance) || (params.min_speed > 0 && distance < params.early_exit_range)) {
            break;
        }

        arcade(params.forwards ? linear_power : -linear_power, angular_power);
        pros::delay(10);
    }

    if (params.min_speed == 0) arcade(0, 0);
}

} // namespace vortex
