#include "vortex/drive/chassis.hpp"

namespace vortex {

void Chassis::setPTO(std::shared_ptr<pros::MotorGroup> left, std::shared_ptr<pros::MotorGroup> right) {
    // Save current motors if they haven't been saved yet
    // Implementation note: This assumes the user might want to swap back
    // For now, we update the active config pointers
    config.left_motors = left;
    config.right_motors = right;
}

void Chassis::resetPTO() {
    // Reset to initial config motors
    // This requires us to have stored the original group pointers
    // I'll update the Chassis class in header to store 'original_left' etc.
}

} // namespace vortex
