#pragma once
#include "vortex/drive/odom.hpp"
#include "vortex/control/pid.hpp"
#include "vortex/util/driveCurve.hpp"
#include "vortex/util/asset.hpp"
#include "pros/motor_group.hpp"
#include <memory>

namespace vortex {

/**
 * @brief Direction for turns
 */
enum class AngularDirection {
    AUTO,
    CW_CLOCKWISE,
    CCW_COUNTERCLOCKWISE
};

/**
 * @brief Parameters for Chassis movements
 */
struct MoveToPointParams {
    bool forwards = true;
    int max_speed = 127;
    int min_speed = 0; // For motion chaining
    double accel = 5.0; // Slew rate
    double decel = 5.0; // Profile decel
    double early_exit_range = 0;
    bool async = false;
};

struct MoveToPoseParams {
    bool forwards = true;
    double lead = 0.6; // Boomerang lead factor
    int max_speed = 127;
    int min_speed = 0;
    double accel = 5.0; // Acceleration (0 to disable)
    double decel = 5.0; // Deceleration (0 to disable)
    double early_exit_range = 0;
    bool async = true;
};

struct TurnParams {
    AngularDirection direction = AngularDirection::AUTO;
    int max_speed = 127;
    int min_speed = 0;
    double early_exit_range = 0;
    bool async = true;
};

struct SwingParams {
    bool left_side = true; // Which side to move (other is locked)
    AngularDirection direction = AngularDirection::AUTO;
    int max_speed = 127;
    int min_speed = 0;
    double early_exit_range = 0;
    bool async = true;
};

struct ChassisConfig {
    std::shared_ptr<pros::MotorGroup> left_motors;
    std::shared_ptr<pros::MotorGroup> right_motors;
    double track_width;
    double wheel_diameter;
    double rpm;
    OdomConfig odom_sensors;
};

class Chassis {
public:
    Chassis(ChassisConfig config);

    /**
     * @brief Setup the chassis (start odom, etc.)
     */
    void initialize();

    // --- Driver Control ---
    void tank(int left, int right, double curve = 0.0);
    void arcade(int forward, int turn, double curve = 0.0);
    void curvature(int throttle, int turn, double curve = 0.0);
    
    // --- PTO Management ---
    void setPTO(std::shared_ptr<pros::MotorGroup> left, std::shared_ptr<pros::MotorGroup> right);
    void resetPTO();
    
    // --- Autonomous Motions ---
    void moveToPoint(double x, double y, int timeout, MoveToPointParams params = {});
    void moveToPose(double x, double y, double theta, int timeout, MoveToPoseParams params = {});
    void turnToHeading(double theta, int timeout, TurnParams params = {});
    void turnToPoint(double x, double y, int timeout, TurnParams params = {});
    void swingToHeading(double theta, int timeout, SwingParams params = {});
    void swingToPoint(double x, double y, int timeout, SwingParams params = {});
    void follow(Asset path, double lookahead, int timeout, bool forwards = true, bool async = true);

    // --- Wait Helpers ---
    void waitUntilDist(double inches);
    void waitUntilAngle(double degrees);
    void waitUntilSettled();

    /**
     * @brief Wait until the current movement is finished
     */
    void waitUntilDone();

    /**
     * @brief Sets the brake mode of the drive motors
     */
    void setBrakeMode(pros::motor_brake_mode_e mode);

    // Access to underlying components
    Odom odom;
    PID linear_pid;
    PID angular_pid;
    
    double desaturate_bias = 0.5;
    bool active_braking = false;

private:
    ChassisConfig config;
    std::shared_ptr<pros::MotorGroup> original_left;
    std::shared_ptr<pros::MotorGroup> original_right;
    
    pros::Task* chassis_task = nullptr;
    double dist_to_target = 0;
    double angle_to_target = 0;
    
    // Default PIDs (will be updated by user)
    static PIDSettings default_linear_settings;
    static PIDSettings default_angular_settings;

    void movement_task(void* param);
    friend void chassis_task_fn(void* param);
};

} // namespace vortex
