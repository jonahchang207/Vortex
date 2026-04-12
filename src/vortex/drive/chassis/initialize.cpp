#include "vortex/drive/chassis.hpp"

namespace vortex {

void Chassis::initialize() {
    odom.start();
    original_left = config.left_motors;
    original_right = config.right_motors;
    
    linear_pid.reset();
    angular_pid.reset();
}

} // namespace vortex
