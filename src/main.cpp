#include "main.h"
#include "vortex/vortex.hpp"

// Motor definitions
std::shared_ptr<pros::MotorGroup> left_motors = std::make_shared<pros::MotorGroup>({1, -2, 3});
std::shared_ptr<pros::MotorGroup> right_motors = std::make_shared<pros::MotorGroup>({-4, 5, -6});

// Chassis Setup with new LemLib-style configuration
vortex::Chassis chassis({
    .left_motors = left_motors,
    .right_motors = right_motors,
    .track_width = 12.5,
    .wheel_diameter = 4.0,
    .rpm = 600,
    .odom_sensors = {
        .imu = std::make_shared<pros::Imu>(20)
    }
});

// Piston for an intake or wing (EZ-Template style)
vortex::Piston wing('A');

// Autonomous Selector (EZ-Template style)
vortex::AutonSelector selector({
    {"Red Left", "Score 2 balls on red left", []() {
        // Example motion using new Boomerang algorithm
        chassis.moveToPose(24, 24, 90, 2000);
        wing.extend();
    }},
    {"Blue Right", "Score 2 balls on blue right", []() {
        // Example motion using Pure Pursuit if path existed
        // chassis.follow(myPath, 10, 3000);
        chassis.moveToPoint(24, 0, 1500);
    }}
});

void initialize() {
    chassis.initialize(); // Calibrates IMU and starts odom task
    selector.initialize(); // Creates brain screen GUI
}

void disabled() {}
void competition_initialize() {}

void autonomous() {
    selector.run(); // Executes the selected routine
}

void opcontrol() {
    pros::Controller master(pros::E_CONTROLLER_MASTER);
    vortex::ExpoDriveCurve drive_curve(5.0, 10.0, 1.05); // Smooth control

    while (true) {
        int forward = master.get_analog(ANALOG_LEFT_Y);
        int turn = master.get_analog(ANALOG_RIGHT_X);
        
        // Use drive curve for better precision
        chassis.arcade(drive_curve.calculate(forward), drive_curve.calculate(turn));
        
        if (master.get_digital_new_press(DIGITAL_A)) {
            wing.toggle();
        }

        pros::delay(20);
    }
}