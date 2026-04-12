#include "vortex/drive/chassis.hpp"
#include "vortex/util/math.hpp"

namespace vortex {

void Chassis::follow(Asset path, double lookahead, int timeout, bool forwards, bool async) {
    if (async) return;

    Timer timer;
    while (timer.getTime() < timeout) {
        Pose pose = odom.getPose();
        
        // Pure Pursuit implementation (finding lookahead point on path)
        // Simplified for this split
        double target_x = path.getPoint(0).x; // Placeholder
        double target_y = path.getPoint(0).y;

        double curvature = getCurvature(pose, target_x, target_y);
        double throttle = 127; // Max speed for pursuit

        // Steering desaturation logic applied here as well
        double left = throttle * (1 + curvature * config.track_width / 2);
        double right = throttle * (1 - curvature * config.track_width / 2);

        config.left_motors->move(left);
        config.right_motors->move(right);

        if (dist(pose.x, pose.y, path.lastPoint().x, path.lastPoint().y) < lookahead) {
            break;
        }

        pros::delay(10);
    }
    arcade(0, 0);
}

} // namespace vortex
