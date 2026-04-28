#pragma once
#include "vortex/vortex.hpp"

// ============================================================
//  Vortex Example Project — subsystems.hpp
//
//  This file exposes your chassis and motors so they can be
//  used in any other file in your project.
// ============================================================

// -------------------------------------------------------
//  Motor Groups
// -------------------------------------------------------
extern std::shared_ptr<pros::MotorGroup> left_motors;
extern std::shared_ptr<pros::MotorGroup> right_motors;

// -------------------------------------------------------
//  Chassis
// -------------------------------------------------------
extern vortex::Chassis chassis;
