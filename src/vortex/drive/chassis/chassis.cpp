#include "vortex/drive/chassis.hpp"

namespace vortex {

// Default PIDs (will be updated by user)
PIDSettings Chassis::default_linear_settings = {0, 0, 0};
PIDSettings Chassis::default_angular_settings = {0, 0, 0};

Chassis::Chassis(ChassisConfig config, ChassisParams params) 
    : odom(config.odom_config),
      linear_pid(params.linear_pid),
      angular_pid(params.angular_pid),
      config(config) {
    original_left = config.left_motors;
    original_right = config.right_motors;
}

} // namespace vortex
