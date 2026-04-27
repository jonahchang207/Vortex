#include "vortex/drive/odom.hpp"
#include "vortex/util/math.hpp"

namespace vortex {

TrackingWheel::TrackingWheel(std::shared_ptr<pros::Rotation> rotation, double wheel_diameter, double offset, double gear_ratio) 
    : rotation(rotation), wheel_diameter(wheel_diameter), offset(offset), gear_ratio(gear_ratio) {}

void TrackingWheel::reset() {
    if (rotation) rotation->reset_position();
    last_dist = 0;
}

double TrackingWheel::getDistance() const {
    if (!rotation) return 0;
    // PROS uses centidegrees for rotation sensors
    double current_pos = rotation->get_position() / 100.0;
    return (current_pos / 360.0) * math::PI * wheel_diameter * gear_ratio;
}

double TrackingWheel::getDistanceTraveled() {
    double current_dist = getDistance();
    double delta = current_dist - last_dist;
    last_dist = current_dist;
    return delta;
}

double TrackingWheel::getOffset() const {
    return offset;
}

} // namespace vortex
