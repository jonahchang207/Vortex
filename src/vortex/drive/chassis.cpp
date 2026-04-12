#include "vortex/drive/chassis.hpp"
#include "vortex/util/math.hpp"
#include "pros/rtos.hpp"
#include <algorithm>
#include <cmath>

namespace vortex {

Chassis::Chassis(ChassisConfig config, ChassisParams params)
    : config(config), params(params), odom(config.odom_config) {}

void Chassis::initialize() {
    odom.start();
}

int Chassis::applyCurve(int input, double curve) {
    if (curve == 0) return input;
    double pct = input / 127.0;
    double result = (std::exp(curve * std::abs(pct)) - 1) / (std::exp(curve) - 1);
    return std::copysign(result * 127.0, input);
}

void Chassis::tank(int left, int right, double curve) {
    int cur_left = applyCurve(left, curve);
    int cur_right = applyCurve(right, curve);
    
    if (config.left_motors) config.left_motors->move(cur_left);
    if (config.right_motors) config.right_motors->move(cur_right);
}

void Chassis::arcade(int forward, int turn, double curve) {
    int cur_fwd = applyCurve(forward, curve);
    int cur_turn = applyCurve(turn, curve);
    
    int left = cur_fwd + cur_turn;
    int right = cur_fwd - cur_turn;
    
    // Scale max values if > 127
    int max_mag = std::max(std::abs(left), std::abs(right));
    if (max_mag > 127) {
        left = left * 127 / max_mag;
        right = right * 127 / max_mag;
    }

    if (config.left_motors) config.left_motors->move(left);
    if (config.right_motors) config.right_motors->move(right);
}

void Chassis::moveToPoint(Pose target, int timeout, bool forward, int max_speed, int min_speed) {
    PID linearPID(params.linear_pid);
    PID angularPID(params.angular_pid);
    
    int start_time = pros::millis();
    
    while (pros::millis() - start_time < timeout) {
        Pose current = odom.getPose();
        
        double distance = current.distance(target);
        if (!forward) distance = -distance;
        
        // Simple completion test
        if (std::abs(distance) < 2.0) break; // Settled distance
        
        double target_heading = current.angleTo(target);
        if (!forward) {
            target_heading = math::wrapAngleDeg(target_heading + 180);
        }
        
        double angle_error = math::wrapAngleDeg(target_heading - current.theta);
        
        double linear_power = linearPID.update(distance);
        double angular_power = angularPID.update(angle_error);
        
        // Clamp speed
        double mag = std::abs(linear_power);
        if (mag > max_speed) linear_power = std::copysign(max_speed, linear_power);
        else if (mag < min_speed && mag > 0) linear_power = std::copysign(min_speed, linear_power);
        
        tank(linear_power + angular_power, linear_power - angular_power);
        
        pros::delay(10);
    }
    
    // Stop motors
    tank(0, 0);
}

void Chassis::turnToHeading(double angle, int timeout, int max_speed, int min_speed) {
    PID angularPID(params.angular_pid);
    
    int start_time = pros::millis();
    
    while (pros::millis() - start_time < timeout) {
        Pose current = odom.getPose();
        double error = math::wrapAngleDeg(angle - current.theta);
        
        if (std::abs(error) < 1.0) break; // Settled angle
        
        double power = angularPID.update(error);
        
        // Clamp speed
        double mag = std::abs(power);
        if (mag > max_speed) power = std::copysign(max_speed, power);
        else if (mag < min_speed && mag > 0) power = std::copysign(min_speed, power);
        
        tank(power, -power);
        
        pros::delay(10);
    }
    
    tank(0, 0);
}

} // namespace vortex
