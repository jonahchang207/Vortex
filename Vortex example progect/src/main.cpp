#include "main.h"

// ============================================================
//  Vortex Example Project — main.cpp
//
//  This is where your main competition functions live.
//  Edit your autonomous and opcontrol loops here.
// ============================================================

void initialize() {
    pros::lcd::initialize();
    pros::lcd::set_text(0, "Vortex Example");

    // Start background tasks and calibrate IMU
    chassis.initialize();
    default_constants(); // Set default PIDs from autons.cpp

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
    chassis.odom.setPose({0.0, 0.0, 90.0});

    // Call one of the auton routines defined in autons.cpp
    drive_example();
}

// ============================================================
//  Operator Control
// ============================================================
void opcontrol() {
    pros::Controller master(pros::E_CONTROLLER_MASTER);
    pros::lcd::set_text(1, "Driver control");

    while (true) {
        int forward = master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int turn    = master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);

        // Split Arcade drive with a 0.4 expo curve
        chassis.arcadeSplit(forward, turn, 0.4);

        // Display current pose on LCD
        vortex::Pose pose = chassis.odom.getPose();
        pros::lcd::print(2, "X:%.1f  Y:%.1f  H:%.1f", pose.x, pose.y, pose.theta);

        pros::delay(20);
    }
}
