#pragma once

namespace vortex {

/**
 * @brief Represents a coordinate in 2D space + heading
 */
struct Pose {
    double x;
    double y;
    double theta; // Expected in degrees for user convenience

    Pose(double x = 0, double y = 0, double theta = 0) 
        : x(x), y(y), theta(theta) {}

    /**
     * @brief Distance to another pose
     */
    double distance(const Pose& other) const;

    /**
     * @brief Angle to another pose in degrees
     */
    double angleTo(const Pose& other) const;
};

} // namespace vortex
