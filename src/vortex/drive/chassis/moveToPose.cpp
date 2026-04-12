#include "vortex/drive/chassis.hpp"
#include "vortex/util/math.hpp"
#include <cmath>

namespace vortex {

void Chassis::moveToPose(double x, double y, double theta, int timeout, MoveToPoseParams params) {
    if (params.async) return;

    Timer timer;
    linear_pid.reset();
    angular_pid.reset();

    while (timer.getTime() < timeout) {
        Pose pose = odom.getPose();
        
        // Boomerang: Target a "carrot point" projected from the target
        double d = dist(pose.x, pose.y, x, y);
        double carrot_x = x - std::cos(theta * M_PI / 180.0) * d * params.lead;
        double carrot_y = y - std::sin(theta * M_PI / 180.0) * d * params.lead;

        double angle = std::atan2(carrot_y - pose.y, carrot_x - pose.x);
        if (!params.forwards) angle += M_PI;
        
        double angle_error = wrapAngle(angle - pose.theta);
        double linear_power = linear_pid.update(d);
        double angular_power = angular_pid.update(angle_error);

        if (linear_pid.isSettled(d) || (params.min_speed > 0 && std::abs(linear_power) < params.min_speed)) {
            break;
        }

        arcade(linear_power, angular_power);
        pros::delay(10);
    }

    if (params.min_speed == 0) arcade(0, 0);
}

} // namespace vortex
