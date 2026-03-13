#include "Vortex/chassis/chassis.hpp"
#include "Vortex/util.hpp"
#include "Vortex/logger/logger.hpp"
#include "pros/rtos.hpp"
#include <cmath>

namespace vortex {

void Chassis::calibratePhysical() {
    if (!sensors.imu) {
        infoSink()->error("calibratePhysical() requires an IMU.");
        return;
    }
    if (!sensors.vertical1) {
        infoSink()->error("calibratePhysical() requires at least one vertical tracking wheel.");
        return;
    }

    infoSink()->info("Starting physical calibration — robot will spin ~10 full rotations.");
    infoSink()->info("Clear the area around the robot.");
    pros::delay(2000);

    // Reset sensors to a clean baseline
    sensors.imu->reset();
    while (sensors.imu->is_calibrating()) pros::delay(10);
    sensors.vertical1->reset();

    // Spin in place for 10 full rotations (3600°)
    const double targetDeg = 3600.0;
    while (std::abs(sensors.imu->get_rotation()) < targetDeg) {
        drivetrain.leftMotors->move(60);
        drivetrain.rightMotors->move(-60);
        pros::delay(10);
    }
    drivetrain.leftMotors->move(0);
    drivetrain.rightMotors->move(0);
    pros::delay(500); // let everything settle

    double actualDeg  = sensors.imu->get_rotation();
    double wheelDist  = std::abs(sensors.vertical1->getDistanceTraveled());
    double wheelOff   = std::abs(sensors.vertical1->getOffset());
    double actualRad  = degToRad(actualDeg);

    // For a robot spinning in place with a tracking wheel at offset `d` from center:
    //   wheelDist = actualRad * effectiveRadius
    //   effectiveRadius = wheelDist / actualRad
    //
    // If scrubFactor is correct:
    //   effectiveRadius = wheelOff * scrubFactor   →   scrubFactor = effectiveRadius / wheelOff
    double effectiveRadius   = wheelDist / actualRad;
    double suggestedScrub    = (wheelOff > 0) ? effectiveRadius / wheelOff : 1.0;
    double suggestedTrackWidth = effectiveRadius * 2.0;

    infoSink()->info("--- Calibration Results ---");
    infoSink()->info("Actual rotation:     " + std::to_string(actualDeg) + " deg");
    infoSink()->info("Wheel travel:        " + std::to_string(wheelDist) + " in");
    infoSink()->info("Wheel offset:        " + std::to_string(wheelOff)  + " in");
    infoSink()->info("Effective radius:    " + std::to_string(effectiveRadius) + " in");
    infoSink()->info("Suggested trackWidth (2-wheel):  " + std::to_string(suggestedTrackWidth));
    infoSink()->info("Suggested scrubFactor:           " + std::to_string(suggestedScrub));
    infoSink()->info("Update Drivetrain with these values and re-run to verify.");
}

} // namespace vortex
