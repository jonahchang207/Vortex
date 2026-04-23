#pragma once
#include "vortex/util/pose.hpp"
#include "pros/imu.hpp"
#include "pros/rotation.hpp"
#include <memory>
#include <vector>

namespace vortex {

/**
 * @brief Represents a tracking wheel (encoder + wheel)
 */
class TrackingWheel {
public:
    /**
     * @brief Construct a new Tracking Wheel object
     * 
     * @param rotation Shared pointer to PROS rotation sensor
     * @param wheel_diameter Diameter of the wheel in inches
     * @param offset Distance from the tracking center (inches)
     * @param gear_ratio Gear ratio (e.g. 1.0 for direct drive)
     */
    TrackingWheel(std::shared_ptr<pros::Rotation> rotation, double wheel_diameter, double offset, double gear_ratio = 1.0);

    /**
     * @brief Get the distance traveled by the wheel in inches
     */
    double getDistanceTraveled();

    /**
     * @brief Get the current total distance in inches (for odom internal use)
     */
    double getDistance() const;

    /**
     * @brief Reset the sensor
     */
    void reset();

    /**
     * @brief Get the offset from tracking center
     */
    double getOffset() const;

private:
    std::shared_ptr<pros::Rotation> rotation;
    double wheel_diameter;
    double offset;
    double gear_ratio;
};

/**
 * @brief Configuration for Odometry sensors
 */
struct OdomConfig {
    std::shared_ptr<TrackingWheel> left_encoder = nullptr;
    std::shared_ptr<TrackingWheel> right_encoder = nullptr;
    std::shared_ptr<TrackingWheel> back_encoder = nullptr;
    std::shared_ptr<TrackingWheel> front_encoder = nullptr;
    std::shared_ptr<pros::Imu> imu = nullptr;
};

/**
 * @brief Odometry class for tracking robot position
 */
class Odom {
public:
    Odom(OdomConfig config);

    /**
     * @brief Starts the odometry tracking task
     */
    void start();

    /**
     * @brief Gets current position and heading
     */
    Pose getPose(bool radians = false);

    /**
     * @brief Get the total distance traveled since odom start or reset
     */
    double getDistanceTraveled();

    /**
     * @brief Reset the distance traveled counter
     */
    void resetDistanceTraveled();

    /**
     * @brief Sets the pose of the robot manually
     */
    void setPose(Pose pose);

    /**
     * @brief Calibrates the IMU if present
     */
    void calibrate();

private:
    OdomConfig config;
    Pose pose;
    double total_dist = 0;
    
    void update();
    friend void odom_task_fn(void* param);
};

} // namespace vortex
