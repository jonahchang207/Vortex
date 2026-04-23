#include "main.h"
#include "vortex/vortex.hpp"

// Motor definitions
std::shared_ptr<pros::MotorGroup> left_motors = std::make_shared<pros::MotorGroup>(std::vector<int8_t>{1, -2, 3});
std::shared_ptr<pros::MotorGroup> right_motors = std::make_shared<pros::MotorGroup>(std::vector<int8_t>{-4, 5, -6});

// Chassis Setup with new LemLib-style configuration
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

// Piston for an intake or wing (EZ-Template style)
vortex::Piston wing('A');

// Autonomous Selector (EZ-Template style)
vortex::AutonSelector selector({
    {"Elite Routine", "Shows off motion chaining", []() {
        // Move to (24, 24) but don't stop (chain to next point)
        chassis.moveToPoint(24, 24, 2000, {.min_speed = 60});
        // Transition smoothly to (48, 0)
        chassis.moveToPoint(48, 0, 2000);
        
        // Use a relative distance wait to trigger a wing
        chassis.turnToHeading(90, 1000, {.async = true});
        chassis.waitUntilAngle(45); // Trigger at half-turn
        wing.extend();
    }},
});

void initialize() {
    chassis.initialize();
    selector.initialize();
}

void disabled() {}
void competition_initialize() {}

void autonomous() {
    selector.run();
}

void opcontrol() {
    pros::Controller master(pros::E_CONTROLLER_MASTER);

    while (true) {
        int forward = master.get_analog(ANALOG_LEFT_Y);
        int turn = master.get_analog(ANALOG_RIGHT_X);
        
        // Use Curvature drive for buttery smooth arcs
        chassis.curvature(forward, turn);
        
        if (master.get_digital_new_press(DIGITAL_A)) {
            wing.toggle();
        }

        // Telemetry for real-time tuning
        vortex::Logger::telemetry("Throttle", forward);

        pros::delay(20);
    }
}
