#include "Vortex/util.hpp"
#include <cmath>
#include <numeric>

namespace vortex {

double slew(double target, double current, double maxStep) {
    if (maxStep <= 0) return target;
    double delta = target - current;
    if (std::abs(delta) > maxStep) delta = sgn(delta) * maxStep;
    return current + delta;
}

double angleError(double target, double position, bool radians, AngularDirection direction) {
    target   = sanitizeAngle(target,   radians);
    position = sanitizeAngle(position, radians);
    const double full = radians ? 2 * M_PI : 360.0;
    const double raw  = target - position;
    switch (direction) {
        case AngularDirection::CW_CLOCKWISE:
            return (raw < 0) ? raw + full : raw;
        case AngularDirection::CCW_COUNTERCLOCKWISE:
            return (raw > 0) ? raw - full : raw;
        default:
            return std::remainder(raw, full); // shortest path, result in (-full/2, full/2]
    }
}

double avg(std::vector<double> values) {
    if (values.empty()) return 0;
    return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
}

double ema(double current, double previous, double smooth) {
    return current * smooth + previous * (1.0 - smooth);
}

double getCurvature(Pose pose, Pose other) {
    // Which side of the robot's heading line is the target on?
    // Positive = target is to the left (CCW turn needed).
    double side = sgn(std::sin(pose.theta) * (other.x - pose.x)
                    - std::cos(pose.theta) * (other.y - pose.y));

    // Distance between the two points
    double d = pose.distance(other);
    if (d < 1e-6) return 0; // coincident points → no curvature

    // Perpendicular distance from `other` to the line defined by pose's heading.
    // We avoid tan(θ) by using the line equation in (a, 1, c) form:
    //   Line: sin(θ)·x - cos(θ)·y + (cos(θ)·py - sin(θ)·px) = 0
    double a  = std::sin(pose.theta);
    double b  = -std::cos(pose.theta);
    double c  = std::cos(pose.theta) * pose.y - std::sin(pose.theta) * pose.x;
    double perpDist = std::abs(a * other.x + b * other.y + c); // = |a*x + b*y + c| / sqrt(a²+b²)
    // Note: sqrt(sin²+cos²) = 1, so no denominator needed.

    // curvature = 2 * perp / d²  (from the inscribed circle formula)
    return side * (2.0 * perpDist) / (d * d);
}

double motionProfile(double dist, double totalDist, double maxVel,
                     double maxAccel, double currentVel) {
    // Standard kinematic: v² = 2·a·s → v = sqrt(2·a·s)
    // accelVel: how fast we can go given distance already traveled
    // decelVel: how fast we can go and still stop by the end
    double accelVel = std::sqrt(2.0 * maxAccel * std::abs(dist));
    double decelVel = std::sqrt(2.0 * maxAccel * std::abs(totalDist - dist));
    return std::max(0.0, std::min({(double)maxVel, accelVel, decelVel}));
}

std::vector<Pose> lineCircleIntersection(Pose p1, Pose p2, Pose center, double radius) {
    std::vector<Pose> hits;
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    double fx = p1.x - center.x;
    double fy = p1.y - center.y;
    double a  = dx*dx + dy*dy;
    double b  = 2*(fx*dx + fy*dy);
    double c  = fx*fx + fy*fy - radius*radius;
    double disc = b*b - 4*a*c;
    if (disc < 0) return hits;

    disc = std::sqrt(disc);
    double t1 = (-b - disc) / (2*a);
    double t2 = (-b + disc) / (2*a);

    // Only accept intersections that lie ON the segment (t in [0,1])
    if (t1 >= 0 && t1 <= 1) hits.push_back(p1 + (p2 - p1) * t1);
    if (t2 >= 0 && t2 <= 1) hits.push_back(p1 + (p2 - p1) * t2);
    return hits;
}

} // namespace vortex
