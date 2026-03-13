#pragma once
#include "pros/motors.hpp"
#include "pros/motor_group.hpp"
#include "pros/adi.hpp"
#include "pros/rotation.hpp"

namespace vortex {

/** Standard VEX omniwheel diameters in inches. */
namespace Omniwheel {
    constexpr double NEW_2       = 2.125;
    constexpr double NEW_275     = 2.75;
    constexpr double OLD_275     = 2.75;
    constexpr double NEW_275_HALF = 2.744;
    constexpr double OLD_275_HALF = 2.74;
    constexpr double NEW_325     = 3.25;
    constexpr double OLD_325     = 3.25;
    constexpr double NEW_325_HALF = 3.246;
    constexpr double OLD_325_HALF = 3.246;
    constexpr double NEW_4       = 4.0;
    constexpr double OLD_4       = 4.18;
    constexpr double NEW_4_HALF  = 3.995;
    constexpr double OLD_4_HALF  = 4.175;
}

/**
 * @brief Abstracts a single tracking wheel encoder.
 *
 * Supports three sensor types:
 *   - pros::Rotation    (36 000 counts/rev — most accurate)
 *   - pros::adi::Encoder (360 counts/rev)
 *   - pros::MotorGroup  (internal motor encoder, least accurate)
 *
 * `distance` is the signed perpendicular offset from the robot's center of
 * rotation to the wheel's contact point, in inches.
 *   - Vertical wheels: positive = left of center, negative = right.
 *   - Horizontal wheels: positive = forward of center, negative = behind.
 */
class TrackingWheel {
public:
    TrackingWheel(pros::adi::Encoder* encoder, double wheelDiameter,
                  double distance, double gearRatio = 1.0);

    TrackingWheel(pros::Rotation* encoder, double wheelDiameter,
                  double distance, double gearRatio = 1.0);

    TrackingWheel(pros::MotorGroup* motors, double wheelDiameter,
                  double distance, double rpm);

    /** Reset encoder position to zero. */
    void reset();

    /** Total signed distance traveled since last reset, in inches. */
    double getDistanceTraveled();

    /** Signed offset from center of rotation, in inches. */
    double getOffset() const;

    /** Returns 1 if backed by a motor group, 0 otherwise. */
    int getType() const;

private:
    double diameter;
    double distance;
    double rpm       = 0;
    double gearRatio = 1.0;
    pros::adi::Encoder* encoder  = nullptr;
    pros::Rotation*     rotation = nullptr;
    pros::MotorGroup*   motors   = nullptr;
};

} // namespace vortex
