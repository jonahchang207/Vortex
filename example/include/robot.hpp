#pragma once
#include "vortex/vortex.hpp"

// ============================================================
//  robot.hpp — Example Robot Configuration
//
//  Edit the values in this file to match your robot's wiring
//  and physical measurements. All other files pull from here.
// ============================================================

namespace robot {

// -------------------------------------------------------
// Drive Motor Ports
// Positive = forward, Negative = reversed
// -------------------------------------------------------
constexpr int LEFT_FRONT_PORT  =  1;
constexpr int LEFT_MIDDLE_PORT =  2;
constexpr int LEFT_BACK_PORT   =  3;

constexpr int RIGHT_FRONT_PORT = -4;
constexpr int RIGHT_MIDDLE_PORT = -5;
constexpr int RIGHT_BACK_PORT  = -6;

// -------------------------------------------------------
// Sensor Ports
// -------------------------------------------------------
constexpr int IMU_PORT = 10;

// Tracking wheels (set to 0 to disable)
constexpr int LEFT_ENCODER_PORT  = 11;
constexpr int RIGHT_ENCODER_PORT = 12;
constexpr int BACK_ENCODER_PORT  = 13;   // Perpendicular (strafe) wheel

// -------------------------------------------------------
// Physical Dimensions  (in inches)
// -------------------------------------------------------
constexpr double TRACK_WIDTH      = 12.5;  // Center-to-center wheel spacing
constexpr double WHEEL_DIAMETER   = 3.25;  // Tracking wheel diameter
constexpr double ENCODER_OFFSET_L =  6.25; // Offset from robot center (left encoder)
constexpr double ENCODER_OFFSET_R =  6.25; // Offset from robot center (right encoder)
constexpr double ENCODER_OFFSET_B =  3.5;  // Offset from robot center (back encoder)

// -------------------------------------------------------
// PID Tuning Constants
// Start with small kP, zero kI / kD, then tune.
// -------------------------------------------------------
constexpr vortex::PIDConstants LINEAR_PID  = { .kP = 8.0,  .kI = 0.02, .kD = 1.5 };
constexpr vortex::PIDConstants ANGULAR_PID = { .kP = 3.5,  .kI = 0.0,  .kD = 0.8 };

} // namespace robot
