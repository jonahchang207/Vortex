#include "vortex/drive/chassis.hpp"
#include "vortex/util/math.hpp"
#include "vortex/util/timer.hpp"

namespace vortex {

void Chassis::pointAndGo(double x, double y, int timeout, PointAndGoParams params) {
    if (params.async) return;

    Timer timer;
    
    // 1. Turn to face the point
    Pose pose = odom.getPose();
    double target_angle = pose.angleTo(Pose(x, y));
    if (!params.forwards) {
        target_angle = math::wrapAngleDeg(target_angle + 180.0);
    }

    TurnParams turn_params;
    turn_params.direction = AngularDirection::AUTO;
    turn_params.max_speed = params.max_speed;
    turn_params.min_speed = params.min_speed;
    turn_params.accel = params.accel;
    turn_params.decel = params.decel;
    turn_params.async = false;

    turnToHeading(target_angle, timeout, turn_params);

    // 2. Drive to the point
    int elapsed = timer.getElapsed();
    if (elapsed < timeout) {
        MoveToPointParams move_params;
        move_params.forwards = params.forwards;
        move_params.max_speed = params.max_speed;
        move_params.min_speed = params.min_speed;
        move_params.accel = params.accel;
        move_params.decel = params.decel;
        move_params.early_exit_range = params.early_exit_range;
        move_params.async = false;

        moveToPoint(x, y, timeout - elapsed, move_params);
    }
}

} // namespace vortex
