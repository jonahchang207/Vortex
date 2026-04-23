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
