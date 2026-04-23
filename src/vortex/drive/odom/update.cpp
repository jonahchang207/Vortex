#include "vortex/drive/odom.hpp"
#include "vortex/util/math.hpp"
#include <cmath>

namespace vortex {

void Odom::update() {
    static double last_imu_heading = 0;
    
    // 1. Get sensor deltas
    double delta_l = config.left_encoder ? config.left_encoder->getDistanceTraveled() : 0;
    double delta_r = config.right_encoder ? config.right_encoder->getDistanceTraveled() : 0;
    double delta_s = config.back_encoder ? config.back_encoder->getDistanceTraveled() : 0;

    // 2. Calculate Heading
    double heading_deg = 0;
    if (config.imu) {
        // IMU returns [0, 360) clockwise. Standard math is counter-clockwise.
        heading_deg = 360.0 - config.imu->get_heading();
    } else if (config.left_encoder && config.right_encoder) {
        // Fallback to tank odom if no IMU
        double track_width = std::abs(config.left_encoder->getOffset()) + std::abs(config.right_encoder->getOffset());
        double delta_theta = (delta_r - delta_l) / track_width;
        heading_deg = pose.theta + (delta_theta * math::RAD_TO_DEG);
    }
    
    double heading_rad = heading_deg * math::DEG_TO_RAD;
    double delta_theta = (heading_deg - last_imu_heading) * math::DEG_TO_RAD;
    last_imu_heading = heading_deg;

    // 3. Calculate Local Displacement
    double local_x = 0;
    double local_y = 0;

    if (delta_theta == 0) {
        local_x = delta_s;
        local_y = (delta_l + delta_r) / 2.0;
    } else {
        // Arc tracking
        local_x = 2.0 * std::sin(delta_theta / 2.0) * ((delta_s / delta_theta) + (config.back_encoder ? config.back_encoder->getOffset() : 0));
        local_y = 2.0 * std::sin(delta_theta / 2.0) * (((delta_l + delta_r) / (2.0 * delta_theta)));
    }

    // 4. Global Displacement
    double avg_heading = heading_rad - (delta_theta / 2.0);
    pose.x += local_y * std::sin(avg_heading) + local_x * std::cos(avg_heading);
    pose.y += local_y * std::cos(avg_heading) - local_x * std::sin(avg_heading);
    pose.theta = math::wrapAngleDeg(heading_deg);
    
    total_dist += std::sqrt(local_x * local_x + local_y * local_y);
}

} // namespace vortex
