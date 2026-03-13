#pragma once
#include "Vortex/chassis/trackingWheel.hpp"
#include "Vortex/pose.hpp"
#include "pros/imu.hpp"

namespace vortex {

/**
 * @brief Arc-based 2D odometry.
 *
 * Runs in a 10 ms background task managed by Chassis.
 * Heading source priority: two tracking pods > IMU > motor encoders.
 */
class Odom {
public:
    Odom(TrackingWheel* vertical1, TrackingWheel* vertical2,
         TrackingWheel* horizontal1, TrackingWheel* horizontal2,
         pros::Imu* imu, double scrubFactor = 1.0);

    /** Integrate one sensor frame. Called every 10 ms by the chassis task. */
    void update();

    /** Current pose. radians=true returns theta in radians, false in degrees. */
    Pose getPose(bool radians = false) const;

    /** Global velocity (inches/sec for x,y; rad or deg/sec for theta). */
    Pose getSpeed(bool radians = false) const;

    /** Robot-frame velocity. */
    Pose getLocalSpeed(bool radians = false) const;

    void setPose(Pose pose);
    void setDriftCompensation(bool enabled);

private:
    Pose pose       = {0, 0, 0};
    Pose speed      = {0, 0, 0};
    Pose localSpeed = {0, 0, 0};

    TrackingWheel* vertical1;
    TrackingWheel* vertical2;
    TrackingWheel* horizontal1;
    TrackingWheel* horizontal2;
    pros::Imu*     imu;
    double         scrubFactor;

    // IMU drift compensation
    bool     driftCompensation = false;
    double   imuDriftRate      = 0;
    uint32_t lastDriftTime     = 0;

    // Previous raw sensor readings (for delta computation)
    double lastVertical1   = 0;
    double lastVertical2   = 0;
    double lastHorizontal1 = 0;
    double lastHorizontal2 = 0;
    double lastTheta       = 0;

    // Previous cumulative positions (for delta computation)
    double prevV = 0;
    double prevH = 0;

    // Velocity estimation timing
    uint32_t lastUpdateMs = 0;
};

} // namespace vortex
