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
     * @param port PROS port for rotation sensor
     * @param wheel_diameter Diameter of the wheel in inches
     * @param offset Distance from the tracking center (inches)
     * @param gear_ratio Gear ratio (e.g. 1.0 for direct drive)
     */
    TrackingWheel(int8_t port, double wheel_diameter, double offset, double gear_ratio = 1.0);

    /**
     * @brief Get the distance traveled by the wheel in inches
     */
    double getDistanceTraveled();

    /**
     * @brief Reset the sensor
     */
    void reset();

    /**
     * @brief Get the offset from tracking center
     */
    double getOffset() const;

private:
    pros::Rotation rotation;
    double wheel_diameter;
    double offset;
    double gear_ratio;
    double last_pos = 0;
};

/**
 * @brief Configuration for Odometry sensors
 */
struct OdomConfig {
    std::shared_ptr<TrackingWheel> vertical1 = nullptr;
    std::shared_ptr<TrackingWheel> vertical2 = nullptr;
    std::shared_ptr<TrackingWheel> horizontal1 = nullptr;
    std::shared_ptr<TrackingWheel> horizontal2 = nullptr;
    std::shared_ptr<pros::Imu> imu = nullptr;

    OdomConfig() = default;
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
