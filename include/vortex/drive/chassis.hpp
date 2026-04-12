#pragma once
#include "vortex/drive/odom.hpp"
#include "vortex/control/pid.hpp"
#include "pros/motor_group.hpp"
#include <memory>

namespace vortex {

struct ChassisConfig {
    std::shared_ptr<pros::MotorGroup> left_motors;
    std::shared_ptr<pros::MotorGroup> right_motors;
    double track_width; // Distance between wheels
    OdomConfig odom_config;
};

// PID constants for different movements
struct ChassisParams {
    PIDConstants linear_pid;
    PIDConstants angular_pid;
};

class Chassis {
public:
    Chassis(ChassisConfig config, ChassisParams params);

    void initialize();

    // --- Control methods ---
    void tank(int left, int right, double curve = 0.0);
    void arcade(int forward, int turn, double curve = 0.0);

    // --- Autonomous methods ---
    
    /**
     * @brief Move to a target point (x, y)
     * @param target Target pose (coordinates)
     * @param timeout Timeout in milliseconds
     * @param forward True to drive forward, false to drive backward (default true)
     * @param max_speed Maximum speed (0 to 127) (default 127)
     * @param min_speed Minimum speed (0 to 127) (default 0)
     */
    void moveToPoint(Pose target, int timeout, bool forward = true, int max_speed = 127, int min_speed = 0);

    /**
     * @brief Turn to a specific heading
     * @param angle Expected angle in degrees
     * @param timeout Timeout in milliseconds
     * @param max_speed Maximum speed (default 127)
     * @param min_speed Minimum speed (default 0)
     */
    void turnToHeading(double angle, int timeout, int max_speed = 127, int min_speed = 0);

    // Access to underlying Odom
    Odom odom;

private:
    ChassisConfig config;
    ChassisParams params;
    
    // Internal helper to apply deadband/curve
    int applyCurve(int input, double curve);
};

} // namespace vortex
