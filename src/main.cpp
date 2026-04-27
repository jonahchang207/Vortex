#include "main.h"
#include "vortex/vortex.hpp"
#include "vortex/util/gui/dashboard.hpp"

// Minimal main.cpp to pass template compilation

std::shared_ptr<pros::MotorGroup> left_motors = std::make_shared<pros::MotorGroup>(std::vector<int8_t>{1, -2, 3});
std::shared_ptr<pros::MotorGroup> right_motors = std::make_shared<pros::MotorGroup>(std::vector<int8_t>{-4, 5, -6});

vortex::Chassis chassis(
    vortex::ChassisConfig{
        .left_motors = left_motors,
        .right_motors = right_motors,
        .track_width = 12.5,
        .wheel_diameter = 4.0,
        .rpm = 600,
        .odom_config = {
            .imu = std::make_shared<pros::Imu>(20)
        }
    },
    vortex::ChassisParams{
        .linear_pid = {.kP = 8.0, .kI = 0, .kD = 1.0},
        .angular_pid = {.kP = 4.0, .kI = 0, .kD = 0.5}
    }
);

vortex::AutonSelector selector({
    {"Test", "Test", []() {}}
});

vortex::Dashboard dashboard(chassis, selector);

void initialize() {
    chassis.initialize();
    dashboard.initialize();
}

void disabled() {}
void competition_initialize() {}

void autonomous() {}

void opcontrol() {}
