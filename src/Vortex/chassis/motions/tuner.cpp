#include "Vortex/chassis/chassis.hpp"
#include "Vortex/util.hpp"
#include "Vortex/logger/logger.hpp"
#include "Vortex/timer.hpp"
#include "pros/rtos.hpp"
#include <cstdio>
#include <cmath>

namespace vortex {

void Chassis::tuneLateral(float dist, int timeout) {
    this->requestMotionStart();
    if (!this->motionRunning) return;

    lateralPID.reset();
    lateralLargeExit.reset();
    lateralSmallExit.reset();

    auto c = lateralPID.getConstants();
    infoSink()->info("--- Tuning Lateral PID (target=" + std::to_string(dist) + " in) ---");
    infoSink()->info("kP=" + std::to_string(c.kP) + " kI=" + std::to_string(c.kI) +
                     " kD=" + std::to_string(c.kD));

    Pose   startPose = getPose(true, true);
    double prevPower = 0;
    Timer  timer(timeout);

    while (!timer.isDone() &&
           !lateralLargeExit.getExit() && !lateralSmallExit.getExit() &&
           this->motionRunning)
    {
        double traveled = getPose(true, true).distance(startPose);
        double error    = dist - traveled;

        double power = lateralPID.update(error);
        lateralLargeExit.update(error);
        lateralSmallExit.update(error);

        power = std::clamp(power, -127.0, 127.0);
        power = slew(power, prevPower, linearSettings.slew);
        prevPower = power;

        // CSV-format output for graphing (copy from PROS terminal to a spreadsheet)
        printf("TUNE_LAT: %u,%.4f,%.4f\n", pros::millis(), error, power);

        drivetrain.leftMotors->move(power);
        drivetrain.rightMotors->move(power);
        pros::delay(10);
    }

    drivetrain.leftMotors->move(0);
    drivetrain.rightMotors->move(0);
    this->endMotion();
    infoSink()->info("Lateral tuning done.");
}

void Chassis::tuneAngular(float heading, int timeout) {
    this->requestMotionStart();
    if (!this->motionRunning) return;

    angularPID.reset();
    angularLargeExit.reset();
    angularSmallExit.reset();

    auto c = angularPID.getConstants();
    infoSink()->info("--- Tuning Angular PID (target=" + std::to_string(heading) + " deg) ---");
    infoSink()->info("kP=" + std::to_string(c.kP) + " kI=" + std::to_string(c.kI) +
                     " kD=" + std::to_string(c.kD));

    double prevPower = 0;
    Timer  timer(timeout);

    while (!timer.isDone() &&
           !angularLargeExit.getExit() && !angularSmallExit.getExit() &&
           this->motionRunning)
    {
        double error = angleError(heading, getPose(false).theta, false);

        double power = angularPID.update(error);
        angularLargeExit.update(error);
        angularSmallExit.update(error);

        power = std::clamp(power, -127.0, 127.0);
        power = slew(power, prevPower, angularSettings.slew);
        prevPower = power;

        printf("TUNE_ANG: %u,%.4f,%.4f\n", pros::millis(), error, power);

        drivetrain.leftMotors->move(power);
        drivetrain.rightMotors->move(-power);
        pros::delay(10);
    }

    drivetrain.leftMotors->move(0);
    drivetrain.rightMotors->move(0);
    this->endMotion();
    infoSink()->info("Angular tuning done.");
}

} // namespace vortex
