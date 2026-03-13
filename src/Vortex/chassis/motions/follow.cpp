#include "Vortex/chassis/chassis.hpp"
#include "Vortex/util.hpp"
#include "Vortex/timer.hpp"
#include "pros/rtos.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <cmath>

namespace vortex {

// Parse CSV path asset: each line is "x,y"
static std::vector<Pose> parsePath(const asset& a) {
    std::vector<Pose> path;
    std::string content(reinterpret_cast<char*>(a.buf), a.size);
    std::istringstream ss(content);
    std::string line;
    while (std::getline(ss, line)) {
        std::istringstream ls(line);
        std::string xs, ys;
        if (std::getline(ls, xs, ',') && std::getline(ls, ys, ',')) {
            try { path.emplace_back(std::stof(xs), std::stof(ys)); }
            catch (...) { continue; } // skip malformed lines
        }
    }
    return path;
}

// Find the farthest path point (by arc length) within `lookahead` of `center`.
// Scanning forward from `startIdx` prevents the robot from latching to a point
// behind it if the path doubles back.
static Pose findLookaheadPoint(const std::vector<Pose>& path,
                               Pose center, double lookahead, int& startIdx) {
    // Walk segments forward, take the intersection closest to the end
    for (int i = startIdx; i < (int)path.size() - 1; i++) {
        auto hits = lineCircleIntersection(path[i], path[i+1], center, lookahead);
        if (!hits.empty()) {
            // If two hits, prefer the one closer to the END of the path (further along)
            Pose best = hits[0];
            if (hits.size() == 2 && hits[1].distance(path.back()) < hits[0].distance(path.back()))
                best = hits[1];
            startIdx = i;
            return best;
        }
    }

    // No intersection found — project onto the nearest segment ahead of startIdx
    // and return the endpoint of that segment. This prevents latching to stale
    // intersections from early in the path.
    double minDist = 1e9;
    Pose   closest = path.back();
    for (int i = startIdx; i < (int)path.size(); i++) {
        double d = center.distance(path[i]);
        if (d < minDist) { minDist = d; closest = path[i]; }
    }
    return closest;
}

void Chassis::follow(const asset& pathAsset, float lookahead, int timeout,
                     bool forwards, bool async) {
    if (async) {
        pros::Task([this, pathAsset, lookahead, timeout, forwards]() {
            follow(pathAsset, lookahead, timeout, forwards, false);
        });
        pros::delay(10);
        return;
    }

    this->requestMotionStart();
    if (!this->motionRunning) return;

    std::vector<Pose> path = parsePath(pathAsset);
    if (path.size() < 2) {
        infoSink()->warn("follow(): path has fewer than 2 points — skipping.");
        this->endMotion();
        return;
    }

    Timer timer(timeout);
    int   segIdx = 0;     // which segment we're searching from (monotonically increasing)
    distTraveled = 0;
    Pose  lastPose = getPose(true, true);

    while (!timer.isDone() && this->motionRunning) {
        Pose pose = getPose(true, true);
        distTraveled += pose.distance(lastPose);
        lastPose = pose;

        // When going in reverse, flip the robot's heading so curvature is computed
        // relative to the "backwards facing" direction.
        if (!forwards) pose.theta = normalizeAngle(pose.theta + M_PI);

        // ── Lookahead point ──────────────────────────────────────────────
        Pose lookaheadPt = findLookaheadPoint(path, pose, lookahead, segIdx);

        // ── Curvature ────────────────────────────────────────────────────
        double curvature = getCurvature(pose, lookaheadPt);
        if (std::isnan(curvature) || std::isinf(curvature)) curvature = 0;

        // ── Velocity controller ──────────────────────────────────────────
        // Speed is limited by:
        //   1. maxVelocity (hard cap)
        //   2. Curvature: slow on tight turns to prevent lateral slipping
        //   3. Endpoint P-controller: ramp down as we approach the final point
        double distToEnd    = pose.distance(path.back());
        double maxSpeed     = linearSettings.maxVelocity;
        double curvatureMax = (std::abs(curvature) > 1e-6)
                            ? std::sqrt(drivetrain.horizontalDrift / std::abs(curvature))
                            : maxSpeed;
        double endSpeed     = distToEnd * linearSettings.kP * 2.0;

        double targetSpeed = std::min({maxSpeed, curvatureMax, endSpeed});
        targetSpeed = std::max(targetSpeed, 10.0); // always keep moving

        // ── Differential drive ───────────────────────────────────────────
        // V_left  = V * (2 + c*T) / 2
        // V_right = V * (2 - c*T) / 2
        double halfTrack = drivetrain.trackWidth;
        double leftSpeed  = targetSpeed * (2.0 + curvature * halfTrack) / 2.0;
        double rightSpeed = targetSpeed * (2.0 - curvature * halfTrack) / 2.0;

        // Normalize if either side exceeds 127
        double peak = std::max(std::abs(leftSpeed), std::abs(rightSpeed));
        if (peak > 127.0) { leftSpeed = leftSpeed / peak * 127.0; rightSpeed = rightSpeed / peak * 127.0; }

        // Flip both sides for reverse travel
        if (!forwards) { leftSpeed = -leftSpeed; rightSpeed = -rightSpeed; }

        drivetrain.leftMotors->move(leftSpeed);
        drivetrain.rightMotors->move(rightSpeed);

        // Done when within 2 inches of path end
        if (distToEnd < 2.0) break;
        pros::delay(10);
    }

    drivetrain.leftMotors->move(0);
    drivetrain.rightMotors->move(0);
    distTraveled = -1;
    this->endMotion();
}

} // namespace vortex
