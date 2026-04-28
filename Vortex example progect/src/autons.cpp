#include "main.h"

// ============================================================
//  Vortex Example Project — autons.cpp
//
//  This file contains your autonomous routines.
// ============================================================

/**
 * @brief Sets default PID settings for all autonomous movements.
 */
void default_constants() {
    chassis.linear_pid.setSettings({.kP = 8.0, .kI = 0.02, .kD = 1.5});
    chassis.angular_pid.setSettings({.kP = 3.5, .kI = 0.0, .kD = 0.8});
}

/**
 * @brief Simple straight drive example.
 */
void drive_example() {
    // Drive 24 inches forward
    chassis.moveToPoint(0.0, 24.0, 3000);
    // Drive back to origin
    chassis.moveToPoint(0.0, 0.0, 3000, {.forwards = false});
}

/**
 * @brief Simple turning example.
 */
void turn_example() {
    // Turn 90 degrees clockwise (East)
    chassis.turnToHeading(90.0, 1500);
    // Turn 90 degrees counter-clockwise back to North
    chassis.turnToHeading(0.0, 1500);
}

/**
 * @brief Sequential drive and turn example.
 */
void drive_and_turn() {
    // Drive to 24 inches north
    chassis.moveToPoint(0.0, 24.0, 3000);
    // Turn 90 degrees right
    chassis.turnToHeading(90.0, 1500);
    // Drive 24 inches east
    chassis.moveToPoint(24.0, 24.0, 3000);
}

/**
 * @brief Advanced Boomerang controller example.
 */
void boomerang_example() {
    // Move to (24, 24) and arrive facing 90 degrees (East)
    chassis.moveToPose(24.0, 24.0, 90.0, 4000);
}
