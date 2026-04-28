#include "main.h"
#include "vortex/vortex.hpp"

// ============================================================
//  Vortex Example Project — main.cpp
//
//  This file demonstrates the core Vortex API in a clean,
//  easy-to-use, single-file configuration.
// ============================================================

// -------------------------------------------------------
//  Motor Groups
// -------------------------------------------------------
auto left_motors = vortex::Motors({1, 2, 3});
auto right_motors = vortex::Motors({-4, -5, -6});

// -------------------------------------------------------
//  Chassis Configuration
// -------------------------------------------------------
vortex::Chassis chassis(
    vortex::ChassisConfig{
        .left_motors = left_motors,
        .right_motors = right_motors,
        .track_width = 12.5,   // Center-to-center wheel spacing in inches
        .wheel_diameter = 3.25,
        .rpm = 600,
        .odom_config = {
            // Tracking wheels (Port, Diameter, Offset from center)
            .left_encoder = vortex::Tracker(11, 3.25, 6.25),
            .right_encoder = vortex::Tracker(12, 3.25, 6.25),
            .back_encoder = vortex::Tracker(13, 3.25, 3.5),
            .imu = vortex::Imu(10)
        }
    },
    vortex::ChassisParams{
        // PID constants: kP, kI, kD
        .linear_pid = {.kP = 8.0, .kI = 0.02, .kD = 1.5},
        .angular_pid = {.kP = 3.5, .kI = 0.0, .kD = 0.8}
    }
);

// ============================================================
//  PROS Competition Functions
// ============================================================

void initialize() {
    pros::lcd::initialize();
    pros::lcd::set_text(0, "Vortex Example");

    // Start background tasks and calibrate IMU
    chassis.initialize();

    pros::lcd::set_text(0, "Vortex Ready");
}

void disabled() {}

void competition_initialize() {}

// ============================================================
//  Autonomous
// ============================================================
void autonomous() {
    // Set starting pose — adjust to match your starting position
    // x, y, heading (0° = facing right / east)
    chassis.odom.setPose(0.0, 0.0, 90.0);

    // 1. Point and Go: turn to face (0, 24) and drive to it
    chassis.pointAndGo(0.0, 24.0, 3000);

    // 2. Turn to face 0° (east)
    chassis.turnToHeading(0.0, 1500);

    // 3. Move to Pose (Boomerang): drive to (24, 24) and end facing 0°
    chassis.moveToPose(24.0, 24.0, 0.0, 3000);

    // 4. Return to origin backwards
    chassis.moveToPoint(0.0, 0.0, 4000, {.forwards = false});

    pros::lcd::set_text(1, "Auto complete!");
}

// ============================================================
//  Operator Control
// ============================================================
void opcontrol() {
    pros::Controller master(pros::E_CONTROLLER_MASTER);
    pros::lcd::set_text(1, "Driver control");

    while (true) {
        int forward = master.get_analog(ANALOG_LEFT_Y);
        int turn    = master.get_analog(ANALOG_RIGHT_X);

        // Split Arcade drive with a 0.4 expo curve
        chassis.arcadeSplit(forward, turn, 0.4);

        // Display current pose on LCD
        vortex::Pose pose = chassis.odom.getPose();
        pros::lcd::print(2, "X:%.1f  Y:%.1f  H:%.1f", pose.x, pose.y, pose.theta);

        pros::delay(20);
    }
}
