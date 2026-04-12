#include "vortex/drive/odom.hpp"
#include <cmath>

namespace vortex {

void Odom::update() {
    // Odometry Math Implementation
    // Simplified for the split
    double delta_l = config.vertical1->getDistance();
    double delta_r = config.vertical1->getDistance(); // Placeholder
    
    double heading = config.imu->get_heading() * M_PI / 180.0;
    double delta_dist = (delta_l + delta_r) / 2.0;

    pose.x += delta_dist * std::cos(heading);
    pose.y += delta_dist * std::sin(heading);
    pose.theta = heading * 180.0 / M_PI;
    
    total_dist += std::abs(delta_dist);
}

} // namespace vortex
