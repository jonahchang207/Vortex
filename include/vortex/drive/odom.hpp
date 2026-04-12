#pragma once
#include "vortex/util/pose.hpp"
#include "pros/imu.hpp"
#include "pros/rotation.hpp"
#include <memory>

namespace vortex {

struct TrackingWheel {
    std::shared_ptr<pros::Rotation> encoder;
    double wheel_diameter;
    double offset; // distance from tracking center
    
    TrackingWheel(std::shared_ptr<pros::Rotation> enc, double diameter, double offset)
        : encoder(enc), wheel_diameter(diameter), offset(offset) {}
        
    double getDistanceTraveled();
    void reset();
private:
    double last_position = 0;
};

struct OdomConfig {
    std::shared_ptr<TrackingWheel> left_encoder;
    std::shared_ptr<TrackingWheel> right_encoder;
    std::shared_ptr<TrackingWheel> back_encoder; // Perpendicular
    std::shared_ptr<pros::Imu> imu;
    
    OdomConfig() : left_encoder(nullptr), right_encoder(nullptr), back_encoder(nullptr), imu(nullptr) {}
};

class Odom {
public:
    Odom(OdomConfig config);

    /**
     * @brief Starts the odometry tracking task
     */
    void start();

    /**
     * @brief Gets current pose of the robot
     */
    Pose getPose() const;

    /**
     * @brief Sets the pose of the robot manually
     */
    void setPose(Pose pose);

private:
    OdomConfig config;
    Pose current_pose;
    
    void update();
    friend void odom_task_fn(void* param);
};

} // namespace vortex
