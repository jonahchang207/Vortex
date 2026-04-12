#include "vortex/drive/chassis.hpp"

namespace vortex {

void Chassis::setBrakeMode(pros::motor_brake_mode_e mode) {
    config.left_motors->set_brake_modes(mode);
    config.right_motors->set_brake_modes(mode);
}

} // namespace vortex
