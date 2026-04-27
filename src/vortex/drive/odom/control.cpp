#include "vortex/drive/odom.hpp"
#include "pros/rtos.hpp"

namespace vortex {

void odom_task_fn(void* param) {
    Odom* odom = static_cast<Odom*>(param);
    while (true) {
        odom->update();
        pros::delay(10);
    }
}

void Odom::start() {
    if (config.imu) last_imu_heading = config.imu->get_heading();
    if (config.left_encoder) config.left_encoder->reset();
    if (config.right_encoder) config.right_encoder->reset();
    if (config.back_encoder) config.back_encoder->reset();
    if (config.front_encoder) config.front_encoder->reset();
    
    pros::Task odom_task(odom_task_fn, this, "Odometry Task");
}

Odom::Odom(OdomConfig config) : config(config) {}

void Odom::setPose(Pose pose) {
    this->pose = pose;
}

void Odom::calibrate() {
    if (config.imu) {
        config.imu->reset();
        while (config.imu->is_calibrating()) {
            pros::delay(10);
        }
    }
}

} // namespace vortex
