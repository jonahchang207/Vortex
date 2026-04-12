#include "vortex/drive/chassis.hpp"
#include "vortex/util/math.hpp"
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

    while (timer.getTime() < timeout) {
        Pose pose = odom.getPose();
        double distance = dist(pose.x, pose.y, x, y);
        double angle = std::atan2(y - pose.y, x - pose.x);

        if (!params.forwards) angle += M_PI;
        double angle_error = wrapAngle(angle - pose.theta);

        double linear_power = linear_pid.update(distance);
        double angular_power = angular_pid.update(std::to_string(angle_error)); // Hypothetical wrapper

        // Exit conditions
        if (linear_pid.isSettled(distance) || (params.min_speed > 0 && std::abs(linear_power) < params.min_speed)) {
            break;
        }

        arcade(linear_power, angular_power);
        pros::delay(10);
    }

    if (params.min_speed == 0) arcade(0, 0);
}

} // namespace vortex
