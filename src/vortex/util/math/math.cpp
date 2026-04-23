#include "vortex/util/math.hpp"
#include "vortex/util/pose.hpp"
#include <cmath>

namespace vortex {
namespace math {

double dist(double x1, double y1, double x2, double y2) {
    return std::hypot(x2 - x1, y2 - y1);
}

double getCurvature(Pose pose, double x, double y) {
    // Convert heading to standard math (CCW radians, 0 is east)
    double heading_rad = pose.theta * DEG_TO_RAD;
    
    // Side of the line (using the heading vector)
    double side = std::sin(heading_rad) * (x - pose.x) - std::cos(heading_rad) * (y - pose.y);
    
    // Distance from the point to the line defined by the robot's pose
    // Line equation: sin(theta) * (x - x0) - cos(theta) * (y - y0) = 0
    double distance_to_line = std::abs(std::sin(heading_rad) * (x - pose.x) - std::cos(heading_rad) * (y - pose.y));
    
    double distance_to_point = dist(pose.x, pose.y, x, y);
    
    if (distance_to_point == 0 || distance_to_line == 0) return 0;
    
    // Radius of the circle tangent to pose and passing through (x, y)
    // R = L^2 / (2 * d) where L is distance to point and d is perpendicular distance to heading line
    double r = (distance_to_point * distance_to_point) / (2.0 * distance_to_line);
    
    if (side < 0) return -1.0 / r;
    return 1.0 / r;
}

} // namespace math
} // namespace vortex
