#include "Vortex/chassis/odom.hpp"
#include "Vortex/util.hpp"
#include "pros/rtos.hpp"
#include <cmath>
#include <algorithm>

namespace vortex {

Odom::Odom(TrackingWheel* v1, TrackingWheel* v2,
           TrackingWheel* h1, TrackingWheel* h2,
           pros::Imu* imu, double scrubFactor)
    : vertical1(v1), vertical2(v2), horizontal1(h1), horizontal2(h2),
      imu(imu), scrubFactor(scrubFactor) {}

void Odom::update() {
    // ── 1. Read raw encoder positions ───────────────────────────────────────
    double v1Raw  = vertical1   ? vertical1->getDistanceTraveled()   : 0;
    double v2Raw  = vertical2   ? vertical2->getDistanceTraveled()   : 0;
    double h1Raw  = horizontal1 ? horizontal1->getDistanceTraveled() : 0;
    double h2Raw  = horizontal2 ? horizontal2->getDistanceTraveled() : 0;
    double imuRaw = imu         ? degToRad(imu->get_rotation())      : 0;

    double dV1  = v1Raw  - lastVertical1;
    double dV2  = v2Raw  - lastVertical2;
    double dH1  = h1Raw  - lastHorizontal1;
    double dH2  = h2Raw  - lastHorizontal2;
    double dImu = imuRaw - lastTheta;

    // ── 2. IMU drift compensation ────────────────────────────────────────────
    // When stationary, the IMU slowly drifts. We estimate the drift rate and
    // subtract it each update so heading doesn't wander at rest.
    if (driftCompensation && imu) {
        bool moving = std::abs(dV1) > 0.001 || std::abs(dV2) > 0.001 ||
                      std::abs(dH1) > 0.001 || std::abs(dH2) > 0.001;

        if (!moving) {
            uint32_t now = pros::millis();
            if (lastDriftTime == 0) {
                lastDriftTime = now;
            } else {
                double dt = (now - lastDriftTime) / 1000.0;
                if (dt > 0) imuDriftRate = ema(dImu / dt, imuDriftRate, 0.01);
            }
            lastDriftTime = now;
        } else {
            lastDriftTime = 0;
        }
        dImu -= imuDriftRate * 0.01; // subtract one 10ms cycle of drift
    }

    // Commit previous values
    lastVertical1   = v1Raw;
    lastVertical2   = v2Raw;
    lastHorizontal1 = h1Raw;
    lastHorizontal2 = h2Raw;
    lastTheta       = imuRaw;

    // ── 3. Heading update ────────────────────────────────────────────────────
    // Priority: two dedicated tracking pods > IMU > motor encoders
    // Two pods give direct differential measurement (most accurate).
    double heading = pose.theta;

    if (vertical1 && vertical2 && !vertical1->getType() && !vertical2->getType()) {
        // Δθ = -(ΔL - ΔR) / effectiveTrackWidth
        // Negative sign: left wheel moving further than right = CW turn
        double track = (vertical1->getOffset() - vertical2->getOffset()) * scrubFactor;
        heading -= (dV1 - dV2) / track;
    } else if (imu) {
        heading += dImu;
    } else if (vertical1 && vertical2) {
        // Motor-encoder fallback
        double track = (vertical1->getOffset() - vertical2->getOffset()) * scrubFactor;
        heading -= (dV1 - dV2) / track;
    }

    double dHeading   = heading - pose.theta;
    double avgHeading = pose.theta + dHeading / 2.0; // midpoint heading for arc integral

    // ── 4. Select translation wheels ────────────────────────────────────────
    // Prefer dedicated (non-motor) tracking pods for accuracy.
    TrackingWheel* vW = nullptr;
    TrackingWheel* hW = nullptr;

    if      (vertical1 && !vertical1->getType()) vW = vertical1;
    else if (vertical2 && !vertical2->getType()) vW = vertical2;
    else if (vertical1)                          vW = vertical1;
    if      (horizontal1) hW = horizontal1;
    else if (horizontal2) hW = horizontal2;

    double rawV = vW ? vW->getDistanceTraveled() : 0;
    double rawH = hW ? hW->getDistanceTraveled() : 0;
    double dY   = rawV - prevV; // forward displacement in robot frame (+forward)
    double dX   = rawH - prevH; // lateral displacement in robot frame (+right)
    prevV = rawV;
    prevH = rawH;

    // ── 5. Arc-based local→global transform ─────────────────────────────────
    // When Δheading ≈ 0, robot moved in a straight line — skip arc formula.
    // Otherwise, fit a circular arc to get chord displacement.
    double localX, localY;
    if (std::abs(dHeading) < 1e-6) {
        localX = dX;
        localY = dY;
    } else {
        double hOff = hW ? hW->getOffset() : 0;
        double vOff = vW ? vW->getOffset() : 0;
        // chord = 2*sin(Δθ/2) * (arc_len/Δθ + offset_from_center)
        localX = 2.0 * std::sin(dHeading / 2.0) * (dX / dHeading + hOff);
        localY = 2.0 * std::sin(dHeading / 2.0) * (dY / dHeading + vOff);
    }

    // ── 6. Global pose update ────────────────────────────────────────────────
    // Convention: θ=0 faces north (+Y), θ increases CW, +X is east.
    //
    // Verification:
    //   θ=0 (north), localY=1 (forward):  x += 0,  y += 1  ✓
    //   θ=0 (north), localX=1 (right):    x += 1,  y += 0  ✓
    //   θ=π/2 (east), localY=1 (forward): x += 1,  y += 0  ✓
    //   θ=π/2 (east), localX=1 (right):   x += 0,  y += -1 ✓  (right of east = south)
    Pose prev = pose;
    pose.x    += localY * std::sin(avgHeading) + localX *  std::cos(avgHeading);
    pose.y    += localY * std::cos(avgHeading) - localX *  std::sin(avgHeading);
    pose.theta = heading;

    // ── 7. Velocity estimation (EMA-filtered) ────────────────────────────────
    uint32_t now = pros::millis();
    double dt = lastUpdateMs ? (now - lastUpdateMs) / 1000.0 : 0.01;
    lastUpdateMs = now;

    if (dt > 0) {
        speed.x     = ema((pose.x     - prev.x)     / dt, speed.x,     0.95);
        speed.y     = ema((pose.y     - prev.y)     / dt, speed.y,     0.95);
        speed.theta = ema((pose.theta - prev.theta) / dt, speed.theta, 0.95);
        localSpeed.x     = ema(localX    / dt, localSpeed.x,     0.95);
        localSpeed.y     = ema(localY    / dt, localSpeed.y,     0.95);
        localSpeed.theta = ema(dHeading  / dt, localSpeed.theta, 0.95);
    }
}

Pose Odom::getPose(bool radians) const {
    return radians ? pose : Pose(pose.x, pose.y, radToDeg(pose.theta));
}

Pose Odom::getSpeed(bool radians) const {
    return radians ? speed : Pose(speed.x, speed.y, radToDeg(speed.theta));
}

Pose Odom::getLocalSpeed(bool radians) const {
    return radians ? localSpeed : Pose(localSpeed.x, localSpeed.y, radToDeg(localSpeed.theta));
}

void Odom::setPose(Pose p)              { pose = p; }
void Odom::setDriftCompensation(bool e) { driftCompensation = e; }

} // namespace vortex
