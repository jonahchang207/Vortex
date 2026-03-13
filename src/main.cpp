#include "main.h"
#include "Vortex/api.hpp"

// ── Hardware ──────────────────────────────────────────────────────────────────

pros::Controller controller(pros::E_CONTROLLER_MASTER);

// Motor groups — negative port = motor is physically reversed
pros::MotorGroup leftMotors ({-1, -2, -3}, pros::MotorGearset::blue);
pros::MotorGroup rightMotors({ 4,  5,  6}, pros::MotorGearset::blue);

// Sensors
pros::Imu      imu(10);
pros::Rotation verticalEnc(-11); // reversed
pros::Rotation horizontalEnc(20);

// Tracking wheels
// Vertical:   2.75" wheel, 2.5" right of center  (negative = right of center)
// Horizontal: 2.75" wheel, 5.75" behind center   (negative = behind center)
vortex::TrackingWheel vertical  (&verticalEnc,   vortex::Omniwheel::NEW_275, -2.5);
vortex::TrackingWheel horizontal(&horizontalEnc, vortex::Omniwheel::NEW_275, -5.75);

// ── Vortex config ─────────────────────────────────────────────────────────────

// Physical drivetrain: 10" track width, 4" wheels, 360 RPM, drift=2, scrub=1.0
vortex::Drivetrain drivetrain(&leftMotors, &rightMotors, 10.0,
                              vortex::Omniwheel::NEW_4, 360, 2.0);

// Lateral PID + exit conditions
vortex::ControllerSettings linearController(
    10,   // kP
     0,   // kI
     3,   // kD
     3,   // windup range (in)
     1,   // small error range (in)
   100,   // small error timeout (ms)
     3,   // large error range (in)
   500,   // large error timeout (ms)
    20    // slew rate
);

// Angular PID + exit conditions
vortex::ControllerSettings angularController(
     2,   // kP
     0,   // kI
    10,   // kD
     3,   // windup range (deg)
     1,   // small error range (deg)
   100,   // small error timeout (ms)
     3,   // large error range (deg)
   500,   // large error timeout (ms)
     0    // slew rate (0 = disabled for turns)
);

// Sensor bundle
vortex::OdomSensors sensors(&vertical, nullptr, &horizontal, nullptr, &imu);

// Chassis
vortex::Chassis chassis(drivetrain, linearController, angularController, sensors);

// Auton selector
vortex::AutonSelector selector;

// ── Autonomous routines ───────────────────────────────────────────────────────

void blueSafe() {
    chassis.setPose(-50, -50, 45);
    chassis.moveToPose(0, 0, 0, 3000);
}

void redSafe() {
    chassis.setPose(50, -50, -45);
    chassis.moveToPose(0, 0, 0, 3000);
}

void skills() {
    chassis.setPose(0, 0, 0);
    chassis.tuneLateral(24);
}

// ── PROS callbacks ────────────────────────────────────────────────────────────

void initialize() {
    pros::lcd::initialize();
    chassis.calibrate();

    selector.addAuton("Blue Safe", blueSafe);
    selector.addAuton("Red Safe",  redSafe);
    selector.addAuton("Skills",    skills);
    selector.initialize();
}

void disabled()             {}
void competition_initialize() {}

ASSET(example_txt); // maps to static/example.txt — use this in chassis.follow()

void autonomous() {
    selector.run();
}

void opcontrol() {
    while (true) {
        int throttle = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int turn     = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
        chassis.arcade(throttle, turn);
        pros::delay(10);
    }
}
