#include "main.h"
#include "robot.hpp"
#include "vortex/vortex.hpp"

// ============================================================
//  Vortex Example Project — main.cpp
//
//  This file demonstrates the core Vortex API:
//    • Chassis setup with odometry and PID
//    • Arcade drive in operator control
//    • Autonomous motion (moveToPoint / turnToHeading)
//
//  Change port numbers and PID constants in include/robot.hpp.
// ============================================================

// -------------------------------------------------------
// Motor Groups
// -------------------------------------------------------
auto left_motors = std::make_shared<pros::MotorGroup>(
    std::initializer_list<int>{
        robot::LEFT_FRONT_PORT,
        robot::LEFT_MIDDLE_PORT,
        robot::LEFT_BACK_PORT
    }
);

auto right_motors = std::make_shared<pros::MotorGroup>(
    std::initializer_list<int>{
        robot::RIGHT_FRONT_PORT,
        robot::RIGHT_MIDDLE_PORT,
        robot::RIGHT_BACK_PORT
    }
);

// -------------------------------------------------------
// Tracking Wheels & IMU
// -------------------------------------------------------
auto left_enc  = std::make_shared<pros::Rotation>(robot::LEFT_ENCODER_PORT);
auto right_enc = std::make_shared<pros::Rotation>(robot::RIGHT_ENCODER_PORT);
auto back_enc  = std::make_shared<pros::Rotation>(robot::BACK_ENCODER_PORT);
auto imu       = std::make_shared<pros::Imu>(robot::IMU_PORT);

auto left_wheel = std::make_shared<vortex::TrackingWheel>(
    left_enc, robot::WHEEL_DIAMETER, robot::ENCODER_OFFSET_L
);
auto right_wheel = std::make_shared<vortex::TrackingWheel>(
    right_enc, robot::WHEEL_DIAMETER, robot::ENCODER_OFFSET_R
);
auto back_wheel = std::make_shared<vortex::TrackingWheel>(
    back_enc, robot::WHEEL_DIAMETER, robot::ENCODER_OFFSET_B
);

// -------------------------------------------------------
// Odometry Configuration
// -------------------------------------------------------
vortex::OdomConfig odom_cfg;  // Populated in initialize()

// -------------------------------------------------------
// Chassis
// -------------------------------------------------------
vortex::ChassisConfig chassis_cfg;  // Populated in initialize()

vortex::ChassisParams chassis_params {
    .linear_pid  = robot::LINEAR_PID,
    .angular_pid = robot::ANGULAR_PID
};

// Global chassis instance — declared here, constructed in initialize()
vortex::Chassis* chassis = nullptr;

// ============================================================
//  PROS Competition Functions
// ============================================================

void initialize() {
    pros::lcd::initialize();
    pros::lcd::set_text(0, "Vortex Example");

    // Calibrate IMU (blocks until done)
    imu->reset(true);

    // Build odom config
    odom_cfg.left_encoder  = left_wheel;
    odom_cfg.right_encoder = right_wheel;
    odom_cfg.back_encoder  = back_wheel;
    odom_cfg.imu           = imu;

    // Build chassis config
    chassis_cfg.left_motors  = left_motors;
    chassis_cfg.right_motors = right_motors;
    chassis_cfg.track_width  = robot::TRACK_WIDTH;
    chassis_cfg.odom_config  = odom_cfg;

    // Construct chassis and start odometry
    chassis = new vortex::Chassis(chassis_cfg, chassis_params);
    chassis->initialize();

    pros::lcd::set_text(0, "Vortex Ready");
}

void disabled() {}

void competition_initialize() {
    // Autonomous selector could go here
}

// ============================================================
//  Autonomous
// ============================================================
void autonomous() {
    if (!chassis) return;

    // Set starting pose — adjust to match your starting position on the field
    // Units: inches for x/y, degrees for theta (0° = facing right / east)
    chassis->odom.setPose({ 0.0, 0.0, 90.0 });

    // --- Example routine ---
    // 1. Drive forward to (0, 24): 24 inches north
    chassis->moveToPoint({ 0.0, 24.0, 90.0 }, 3000);

    // 2. Turn to face 0° (east)
    chassis->turnToHeading(0.0, 1500);

    // 3. Drive to (24, 24): 24 inches east
    chassis->moveToPoint({ 24.0, 24.0, 0.0 }, 3000);

    // 4. Return to origin
    chassis->moveToPoint({ 0.0, 0.0, 90.0 }, 4000);

    pros::lcd::set_text(1, "Auto complete!");
}

// ============================================================
//  Operator Control
// ============================================================
void opcontrol() {
    if (!chassis) return;

    pros::Controller master(pros::E_CONTROLLER_MASTER);

    pros::lcd::set_text(1, "Driver control");

    while (true) {
        int forward = master.get_analog(ANALOG_LEFT_Y);   // Left stick up/down
        int turn    = master.get_analog(ANALOG_RIGHT_X);  // Right stick left/right

        // Arcade drive. Increase the curve value (0.0–1.0) for expo response
        chassis->arcade(forward, turn, 0.4);

        // Display current pose on LCD
        vortex::Pose pose = chassis->odom.getPose();
        pros::lcd::print(2, "X:%.1f  Y:%.1f  H:%.1f",
                         pose.x, pose.y, pose.theta);

        pros::delay(20);
    }
}
