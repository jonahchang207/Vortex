#pragma once
#include "pros/motor_group.hpp"
#include "pros/rotation.hpp"
#include "pros/imu.hpp"
#include "vortex/drive/odom.hpp"
#include <memory>
#include <initializer_list>

namespace vortex {

/**
 * @brief Helper to easily create a PROS Motor Group
 * @param ports List of motor ports (negative for reversed)
 * @return std::shared_ptr<pros::MotorGroup>
 */
inline std::shared_ptr<pros::MotorGroup> Motors(std::initializer_list<int8_t> ports) {
    return std::make_shared<pros::MotorGroup>(ports);
}

/**
 * @brief Helper to easily create a Tracking Wheel
 * @param port Rotation sensor port
 * @param wheel_diameter Wheel diameter in inches
 * @param offset Distance from tracking center in inches
 * @param gear_ratio External gear ratio (optional, default 1.0)
 * @return std::shared_ptr<TrackingWheel>
 */
inline std::shared_ptr<TrackingWheel> Tracker(int port, double wheel_diameter, double offset, double gear_ratio = 1.0) {
    return std::make_shared<TrackingWheel>(std::make_shared<pros::Rotation>(port), wheel_diameter, offset, gear_ratio);
}

/**
 * @brief Helper to easily create a PROS IMU
 * @param port IMU sensor port
 * @return std::shared_ptr<pros::Imu>
 */
inline std::shared_ptr<pros::Imu> Imu(int port) {
    return std::make_shared<pros::Imu>(port);
}

} // namespace vortex
