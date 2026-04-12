#include "vortex/drive/odom.hpp"
#include "pros/rtos.hpp"
#include <cmath>

namespace vortex {

void odom_task_fn(void* param) {
    Odom* odom = static_cast<Odom*>(param);
    while (true) {
        odom->update();
        pros::delay(10);
    }
}

double TrackingWheel::getDistanceTraveled() {
    if (!encoder) return 0;
    // Assuming rotation sensor reports in centidegrees
    double current_position = encoder->get_position() / 100.0;
    double delta = current_position - last_position;
    last_position = current_position;
    
    // Convert degrees to linear distance
    return (delta / 360.0) * (wheel_diameter * M_PI);
}

void TrackingWheel::reset() {
    if (encoder) {
        encoder->reset_position();
    }
    last_position = 0;
}

Odom::Odom(OdomConfig config) : config(config) {}

void Odom::start() {
    if (config.left_encoder) config.left_encoder->reset();
    if (config.right_encoder) config.right_encoder->reset();
    if (config.back_encoder) config.back_encoder->reset();
    
    pros::Task odom_task(odom_task_fn, this, "Vortex Odom Task");
}

void Odom::update() {
    // simplified odometry math
    double delta_l = config.left_encoder ? config.left_encoder->getDistanceTraveled() : 0;
    double delta_r = config.right_encoder ? config.right_encoder->getDistanceTraveled() : 0;
    double delta_b = config.back_encoder ? config.back_encoder->getDistanceTraveled() : 0;

    double delta_theta = 0;
    if (config.imu && !std::isinf(config.imu->get_rotation())) {
        double heading = config.imu->get_rotation(); // continuous rotation
        delta_theta = (heading - current_pose.theta) * (M_PI / 180.0);
        current_pose.theta = heading;
    } else if (config.left_encoder && config.right_encoder) {
        // Fallback to encoder odometry for heading
        double dist_between_wheels = config.left_encoder->offset + config.right_encoder->offset;
        delta_theta = (delta_l - delta_r) / dist_between_wheels;
        current_pose.theta += delta_theta * (180.0 / M_PI);
    }

    double delta_x = 0;
    double delta_y = 0;

    if (delta_theta == 0) {
        delta_x = delta_b;
        delta_y = config.right_encoder ? delta_r : delta_l;
    } else {
        double r_r = (config.right_encoder ? delta_r : delta_l) / delta_theta;
        double r_b = delta_b / delta_theta;
        
        double right_offset = config.right_encoder ? config.right_encoder->offset : config.left_encoder->offset;
        double back_offset = config.back_encoder ? config.back_encoder->offset : 0;

        delta_y = 2 * std::sin(delta_theta / 2) * (r_r + right_offset);
        delta_x = 2 * std::sin(delta_theta / 2) * (r_b + back_offset);
    }

    // Global coordinate transformation
    double avg_theta = (current_pose.theta * (M_PI / 180.0)) - (delta_theta / 2);
    current_pose.x += delta_x * std::cos(avg_theta) - delta_y * std::sin(avg_theta);
    current_pose.y += delta_x * std::sin(avg_theta) + delta_y * std::cos(avg_theta);
}

Pose Odom::getPose() const {
    return current_pose;
}

void Odom::setPose(Pose pose) {
    current_pose = pose;
    if (config.imu) {
        config.imu->set_rotation(pose.theta);
    }
}

} // namespace vortex
