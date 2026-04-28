#include "vortex/drive/chassis.hpp"
#include "vortex/util/math.hpp"
#include "vortex/util/timer.hpp"
#include <cmath>

namespace vortex {

void Chassis::moveToPose(double x, double y, double theta, int timeout, MoveToPoseParams params) {
    if (params.async) return;

    Timer timer;
    linear_pid.reset();
    angular_pid.reset();

    target_x = x;
    target_y = y;

    TrapezoidalProfile profile(params.max_speed, params.accel, params.decel);
    double last_speed = 0;

    while (timer.getElapsed() < (uint32_t)timeout) {
        Pose pose = odom.getPose();
        
        // Boomerang: Target a "carrot point" projected from the target
        double d = math::dist(pose.x, pose.y, x, y);
        // Convert theta from 0=North CW to standard math for cos/sin
        double theta_std = (90.0 - theta) * math::DEG_TO_RAD;
        double carrot_x = x - std::cos(theta_std) * d * params.lead;
        double carrot_y = y - std::sin(theta_std) * d * params.lead;

        double target_angle = pose.angleTo(Pose(carrot_x, carrot_y));
        if (!params.forwards) target_angle = math::wrapAngleDeg(target_angle + 180);
        
        double angle_error = math::wrapAngleDeg(target_angle - pose.theta);

        dist_to_target = d;
        angle_to_target = angle_error;

        double linear_power = linear_pid.update(d);
        double profile_speed = profile.calculate(d, last_speed);

        // Clamp PID output to profile
        if (linear_power > profile_speed) linear_power = profile_speed;
        
        // Ensure min_speed for chaining
        if (linear_power < params.min_speed) linear_power = params.min_speed;
        
        last_speed = linear_power;

        double angular_power = angular_pid.update(angle_error);

        if (linear_pid.isSettled(d) || (params.min_speed > 0 && d < params.early_exit_range)) {
            break;
        }

        arcade(params.forwards ? linear_power : -linear_power, angular_power);
        pros::delay(10);
    }

    if (params.min_speed == 0) arcade(0, 0);
}

} // namespace vortex
