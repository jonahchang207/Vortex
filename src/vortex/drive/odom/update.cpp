#include "vortex/drive/odom.hpp"
#include "vortex/util/math.hpp"
#include <cmath>

namespace vortex {

void Odom::update() {
    // 1. Get sensor deltas
    double delta_l = config.left_encoder ? config.left_encoder->getDistanceTraveled() : 0;
    double delta_r = config.right_encoder ? config.right_encoder->getDistanceTraveled() : 0;
    double delta_s = config.back_encoder ? config.back_encoder->getDistanceTraveled() : 0;

    // 2. Calculate Heading
    double heading_deg = pose.theta;
    if (config.imu) {
        // IMU returns [0, 360) clockwise. 0 is North, 90 is East.
        // This matches our coordinate system (0=North, 90=East CW).
        heading_deg = config.imu->get_heading();
    } else if (config.left_encoder && config.right_encoder) {
        // Fallback to tank odom if no IMU
        // In CW system, if left wheel moves more than right, theta increases
        double track_width = std::abs(config.left_encoder->getOffset()) + std::abs(config.right_encoder->getOffset());
        double delta_theta_deg = (delta_l - delta_r) / track_width * math::RAD_TO_DEG;
        heading_deg += delta_theta_deg;
    }
    
    double delta_theta_deg = math::wrapAngleDeg(heading_deg - last_imu_heading);
    double avg_heading_deg = last_imu_heading + (delta_theta_deg / 2.0);
    double avg_heading_rad = avg_heading_deg * math::DEG_TO_RAD;
    double delta_theta_rad = delta_theta_deg * math::DEG_TO_RAD;
    last_imu_heading = heading_deg;

    // 3. Calculate Local Displacement
    double local_x = 0;
    double local_y = 0;

    if (std::abs(delta_theta_rad) < 0.001) {
        local_x = delta_s;
        local_y = (delta_l + delta_r) / 2.0;
    } else {
        // Arc tracking
        local_x = 2.0 * std::sin(delta_theta_rad / 2.0) * ((delta_s / delta_theta_rad) + (config.back_encoder ? config.back_encoder->getOffset() : 0));
        local_y = 2.0 * std::sin(delta_theta_rad / 2.0) * (((delta_l + delta_r) / (2.0 * delta_theta_rad)));
    }

    // 4. Global Displacement
    // Matrix for 0=North, 90=East CW
    pose.x += local_y * std::sin(avg_heading_rad) + local_x * std::cos(avg_heading_rad);
    pose.y += local_y * std::cos(avg_heading_rad) - local_x * std::sin(avg_heading_rad);
    pose.theta = math::wrapAngleDeg(heading_deg);
    
    total_dist += std::sqrt(local_x * local_x + local_y * local_y);
}

} // namespace vortex
