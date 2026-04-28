#include "main.h"

// ============================================================
//  Vortex Example Project — subsystems.cpp
//
//  This file is where you configure your robot's hardware.
//  Keep this file clean to make tuning easy!
// ============================================================

// -------------------------------------------------------
//  Motor Groups
// -------------------------------------------------------
// Define your motor ports here. Negative ports mean reversed.
std::shared_ptr<pros::MotorGroup> left_motors = vortex::Motors({1, 2, 3});
std::shared_ptr<pros::MotorGroup> right_motors = vortex::Motors({-4, -5, -6});

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
            // Tracking wheels: Port, Diameter, Offset from center
            .left_encoder = vortex::Tracker(11, 3.25, 6.25),
            .right_encoder = vortex::Tracker(12, 3.25, 6.25),
            .back_encoder = vortex::Tracker(13, 3.25, 3.5),
            
            // IMU: Port
            .imu = vortex::Imu(10)
        }
    },
    vortex::ChassisParams{
        // PID constants: kP, kI, kD
        .linear_pid = {.kP = 8.0, .kI = 0.02, .kD = 1.5},
        .angular_pid = {.kP = 3.5, .kI = 0.0, .kD = 0.8}
    }
);
