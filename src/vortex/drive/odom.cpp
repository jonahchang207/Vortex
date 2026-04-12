#include "vortex/drive/odom.hpp"
#include "pros/rtos.hpp"
#include <cmath>

namespace vortex {

TrackingWheel::TrackingWheel(int8_t port, double wheel_diameter, double offset, double gear_ratio)
    : rotation(port), wheel_diameter(wheel_diameter), offset(offset), gear_ratio(gear_ratio) {}

double TrackingWheel::getDistanceTraveled() {
    double current_pos = rotation.get_position() / 100.0; // centidegrees to degrees
    double delta = current_pos - last_pos;
    last_pos = current_pos;
    
    // Convert degrees to linear distance: (degrees / 360) * diameter * PI * gear_ratio
    return (delta / 360.0) * (wheel_diameter * M_PI) * gear_ratio;
}

void TrackingWheel::reset() {
    rotation.reset_position();
    last_pos = 0;
}

double TrackingWheel::getOffset() const {
    return offset;
}

void odom_task_fn(void* param) {
    Odom* odom = static_cast<Odom*>(param);
    while (true) {
        odom->update();
        pros::delay(10);
    }
}

Odom::Odom(OdomConfig config) : config(config) {}

void Odom::start() {
    if (config.vertical1) config.vertical1->reset();
    if (config.vertical2) config.vertical2->reset();
    if (config.horizontal1) config.horizontal1->reset();
    if (config.horizontal2) config.horizontal2->reset();
    
    pros::Task odom_task(odom_task_fn, this, "Vortex Odom Task");
}

void Odom::update() {
    // 1. Calculate deltas for all wheels
    double delta_v1 = config.vertical1 ? config.vertical1->getDistanceTraveled() : 0;
    double delta_v2 = config.vertical2 ? config.vertical2->getDistanceTraveled() : 0;
    double delta_h1 = config.horizontal1 ? config.horizontal1->getDistanceTraveled() : 0;
    double delta_h2 = config.horizontal2 ? config.horizontal2->getDistanceTraveled() : 0;

    // 2. Calculate heading change
    double delta_theta = 0;
    if (config.imu && !std::isinf(config.imu->get_rotation())) {
        double current_heading = config.imu->get_rotation();
        delta_theta = (current_heading - current_pose.theta) * (M_PI / 180.0);
        current_pose.theta = current_heading;
    } else if (config.vertical1 && config.vertical2) {
        delta_theta = (delta_v1 - delta_v2) / (config.vertical1->getOffset() - config.vertical2->getOffset());
        current_pose.theta += delta_theta * (180.0 / M_PI);
    }

    // 3. Coordinate transformation (Arc-based tracking)
    double delta_x = 0;
    double delta_y = 0;

    // Use vertical1 if available, else vertical2 for forward/backward
    double delta_v = config.vertical1 ? delta_v1 : delta_v2;
    double v_offset = config.vertical1 ? config.vertical1->getOffset() : (config.vertical2 ? config.vertical2->getOffset() : 0);
    
    // Use horizontal1 if available, else horizontal2 for left/right
    double delta_h = config.horizontal1 ? delta_h1 : delta_h2;
    double h_offset = config.horizontal1 ? config.horizontal1->getOffset() : (config.horizontal2 ? config.horizontal2->getOffset() : 0);

    if (delta_theta == 0) {
        delta_x = delta_h;
        delta_y = delta_v;
    } else {
        delta_x = 2 * std::sin(delta_theta / 2) * (delta_h / delta_theta + h_offset);
        delta_y = 2 * std::sin(delta_theta / 2) * (delta_v / delta_theta - v_offset);
    }

    double avg_theta = (current_pose.theta * (M_PI / 180.0)) - (delta_theta / 2);
    current_pose.x += delta_x * std::cos(avg_theta) + delta_y * std::sin(avg_theta);
    current_pose.y += delta_x * std::sin(avg_theta) - delta_y * std::cos(avg_theta);
}

Pose Odom::getPose() const {
    return current_pose;
}

void Odom::setPose(Pose pose) {
    current_pose = pose;
    if (config.imu) config.imu->set_rotation(pose.theta);
}

void Odom::calibrate() {
    if (config.imu) {
        config.imu->reset();
        while (config.imu->is_calibrating()) pros::delay(10);
    }
}

} // namespace vortex
