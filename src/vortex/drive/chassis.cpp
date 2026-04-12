#include "vortex/drive/chassis.hpp"
#include "vortex/util/math.hpp"
#include "vortex/control/profile.hpp"
#include "pros/rtos.hpp"
#include <cmath>
#include <algorithm>

namespace vortex {

PIDSettings Chassis::default_linear_settings = {10.0, 0, 0, 0, 0};
PIDSettings Chassis::default_angular_settings = {1.0, 0, 0, 0, 0};

Chassis::Chassis(ChassisConfig config)
    : config(config), odom(config.odom_sensors), 
      linear_pid(default_linear_settings), 
      angular_pid(default_angular_settings) {}

void Chassis::initialize() {
    odom.start();
}

void Chassis::setBrakeMode(pros::motor_brake_mode_e mode) {
    if (config.left_motors) config.left_motors->set_brake_mode(mode);
    if (config.right_motors) config.right_motors->set_brake_mode(mode);
}

void Chassis::tank(int left, int right, double curve) {
    if (config.left_motors) config.left_motors->move(left);
    if (config.right_motors) config.right_motors->move(right);
}

void Chassis::arcade(int forward, int turn, double curve) {
    int left = forward + turn;
    int right = forward - turn;
    
    // Steering Desaturation: Prioritize turn when saturated
    int max_val = std::max(std::abs(left), std::abs(right));
    if (max_val > 127) {
        double scale = 127.0 / max_val;
        left *= scale;
        right *= scale;
    }
    
    tank(left, right, curve);
}

void Chassis::waitUntilDist(double inches) {
    while (is_moving && std::abs(dist_to_target) > inches) pros::delay(10);
}

void Chassis::waitUntilAngle(double degrees) {
    while (is_moving && std::abs(angle_to_target) > degrees) pros::delay(10);
}

void Chassis::waitUntilSettled() {
    waitUntilDone();
}

void Chassis::waitUntilDone() {
    while (is_moving) pros::delay(10);
}

// --- Internal Task Handling ---

struct MotionData {
    Chassis* chassis;
    double x, y, theta;
    int timeout;
    enum { MOVE_POINT, MOVE_POSE, TURN_HEADING, TURN_POINT, SWING_HEADING, SWING_POINT } type;
    void* params;
};

void chassis_task_fn(void* param) {
    MotionData* data = static_cast<MotionData*>(param);
    // Implementation of specific motions goes here...
    // To keep it simple for this template, we'll implement them inline or as private members
    delete data;
}

// --- Movement Implementation ---

void Chassis::turnToHeading(double theta, int timeout, TurnParams params) {
    is_moving = true;
    uint32_t start_time = pros::millis();
    angular_pid.reset();
    angular_pid.setExitCondition({0.5, 50, timeout}); // Default: 0.5 deg error, 50ms settle

    while (pros::millis() - start_time < timeout) {
        double current_theta = odom.getPose().theta;
        double error = math::wrapAngleDeg(theta - current_theta);
        angle_to_target = error;

        // Handle direction
        if (params.direction == AngularDirection::CW_CLOCKWISE && error < 0) error += 360;
        if (params.direction == AngularDirection::CCW_COUNTERCLOCKWISE && error > 0) error -= 360;

        if (angular_pid.isSettled(error)) break;

        double power = angular_pid.update(error);
        
        // Clamp speed
        if (std::abs(power) > params.max_speed) power = std::copysign(params.max_speed, power);
        if (std::abs(power) < params.min_speed && std::abs(power) > 0) power = std::copysign(params.min_speed, power);

        tank(power, -power);
        pros::delay(10);
    }
    tank(0, 0);
    is_moving = false;
}

void Chassis::moveToPoint(double x, double y, int timeout, MoveToPointParams params) {
    is_moving = true;
    uint32_t start_time = pros::millis();
    linear_pid.reset();
    angular_pid.reset();
    linear_pid.setExitCondition({1.0, 50, timeout}); // 1 inch error

    TrapezoidalProfile profile(params.max_speed, params.accel, params.decel);
    double current_speed = 0;

    while (pros::millis() - start_time < timeout) {
        Pose current = odom.getPose();
        double dist = std::hypot(x - current.x, y - current.y);
        double target_heading = std::atan2(x - current.x, y - current.y) * math::RAD_TO_DEG;
        dist_to_target = dist;
        angle_to_target = math::wrapAngleDeg(target_heading - current.theta);
        
        if (!params.forwards) {
            dist = -dist;
            target_heading = math::wrapAngleDeg(target_heading + 180);
        }

        if (linear_pid.isSettled(dist)) break;
        if (params.early_exit_range > 0 && std::abs(dist) < params.early_exit_range) break;

        double angle_error = math::wrapAngleDeg(target_heading - current.theta);
        
        double linear_power = linear_pid.update(dist);
        double angular_power = angular_pid.update(angle_error);

        // Apply Trapezoidal Profile
        double profiled_speed = profile.calculate(dist, std::abs(current_speed));
        if (std::abs(linear_power) > profiled_speed) {
            linear_power = std::copysign(profiled_speed, linear_power);
        }
        
        // Ensure min_speed
        if (std::abs(linear_power) < params.min_speed && std::abs(linear_power) > 0) {
            linear_power = std::copysign(params.min_speed, linear_power);
        }

        current_speed = linear_power; // Approximate
        tank(linear_power + angular_power, linear_power - angular_power);
        pros::delay(10);
    }
    tank(0, 0);
    is_moving = false;
}

void Chassis::moveToPose(double x, double y, double theta, int timeout, MoveToPoseParams params) {
    is_moving = true;
    uint32_t start_time = pros::millis();
    linear_pid.reset();
    angular_pid.reset();

    TrapezoidalProfile profile(params.max_speed, params.accel, params.decel);
    double current_speed = 0;
    
    while (pros::millis() - start_time < timeout) {
        Pose current = odom.getPose();
        
        // Boomerang algorithm: Calculate carrot point
        double dist = std::hypot(x - current.x, y - current.y);
        double carrot_x = x - std::sin(theta * math::DEG_TO_RAD) * dist * params.lead;
        double carrot_y = y - std::cos(theta * math::DEG_TO_RAD) * dist * params.lead;

        double carrot_dist = std::hypot(carrot_x - current.x, carrot_y - current.y);
        double carrot_heading = std::atan2(carrot_x - current.x, carrot_y - current.y) * math::RAD_TO_DEG;

        dist_to_target = dist;
        angle_to_target = math::wrapAngleDeg(carrot_heading - current.theta);

        if (!params.forwards) {
            carrot_dist = -carrot_dist;
            carrot_heading = math::wrapAngleDeg(carrot_heading + 180);
        }

        if (dist < 1.0) break; // Settled near target point

        double angle_error = math::wrapAngleDeg(carrot_heading - current.theta);
        
        double linear_power = linear_pid.update(carrot_dist);
        double angular_power = angular_pid.update(angle_error);

        // Apply Trapezoidal Profile based on distance to ACTUAL target
        double profiled_speed = profile.calculate(dist, std::abs(current_speed));
        if (std::abs(linear_power) > profiled_speed) {
            linear_power = std::copysign(profiled_speed, linear_power);
        }

        current_speed = linear_power; // Approximate
        tank(linear_power + angular_power, linear_power - angular_power);
        pros::delay(10);
    }
    tank(0, 0);
    is_moving = false;
}

// Turn to point is just turn to heading with atan2
void Chassis::turnToPoint(double x, double y, int timeout, TurnParams params) {
    Pose current = odom.getPose();
    double target_heading = std::atan2(x - current.x, y - current.y) * math::RAD_TO_DEG;
    turnToHeading(target_heading, timeout, params);
}

void Chassis::swingToHeading(double theta, int timeout, SwingParams params) {
    is_moving = true;
    uint32_t start_time = pros::millis();
    angular_pid.reset();

    while (pros::millis() - start_time < timeout) {
        double error = math::wrapAngleDeg(theta - odom.getPose().theta);
        if (angular_pid.isSettled(error)) break;

        double power = angular_pid.update(error);
        if (params.left_side) tank(power, 0);
        else tank(0, -power);

        pros::delay(10);
    }
    tank(0, 0);
    is_moving = false;
}

void Chassis::swingToPoint(double x, double y, int timeout, SwingParams params) {
    Pose current = odom.getPose();
    double target_heading = std::atan2(x - current.x, y - current.y) * math::RAD_TO_DEG;
    swingToHeading(target_heading, timeout, params);
}

void Chassis::follow(Asset path, double lookahead, int timeout, bool forwards, bool async) {
    is_moving = true;
    uint32_t start_time = pros::millis();
    
    // Simple CSV parser for path data
    std::vector<Pose> points;
    std::string data(reinterpret_cast<const char*>(path.data), path.size);
    size_t pos = 0;
    while ((pos = data.find('\n')) != std::string::npos) {
        std::string line = data.substr(0, pos);
        size_t comma = line.find(',');
        if (comma != std::string::npos) {
            double px = std::stod(line.substr(0, comma));
            double py = std::stod(line.substr(comma + 1));
            points.push_back({px, py, 0});
        }
        data.erase(0, pos + 1);
    }

    int last_found_index = 0;
    while (pros::millis() - start_time < timeout) {
        Pose current = odom.getPose();
        
        // Find lookahead point
        Pose lookahead_point = points.back();
        for (int i = last_found_index; i < points.size() - 1; i++) {
            double d = std::hypot(points[i].x - current.x, points[i].y - current.y);
            if (d > lookahead) {
                lookahead_point = points[i];
                last_found_index = i;
                break;
            }
        }

        double dist = std::hypot(lookahead_point.x - current.x, lookahead_point.y - current.y);
        if (dist < 2.0 && last_found_index >= points.size() - 2) break;

        double target_heading = std::atan2(lookahead_point.x - current.x, lookahead_point.y - current.y) * math::RAD_TO_DEG;
        if (!forwards) target_heading = math::wrapAngleDeg(target_heading + 180);

        double angle_error = math::wrapAngleDeg(target_heading - current.theta);
        double linear_power = linear_pid.update(lookahead); // Pure pursuit usually moves at constant speed or fixed voltage
        double angular_power = angular_pid.update(angle_error);

        tank(linear_power + angular_power, linear_power - angular_power);
        pros::delay(10);
    }
    tank(0, 0);
    is_moving = false;
}

} // namespace vortex
