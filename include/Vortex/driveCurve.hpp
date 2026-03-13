#pragma once

namespace vortex {

/** Abstract interface for a joystick input curve. */
class DriveCurve {
public:
    virtual ~DriveCurve() = default;
    /** Map raw joystick input [-127, 127] to a curved output [-127, 127]. */
    virtual double curve(double input) = 0;
};

/**
 * @brief Exponential drive curve.
 *
 * Produces a curve that is more sensitive near the center and more aggressive
 * near the edges. Useful for precise slow maneuvers without sacrificing top speed.
 *
 * Curve shape is controlled by `curveGain`. Higher values → more exponential.
 */
class ExpoDriveCurve : public DriveCurve {
public:
    /**
     * @param deadband   Input range near zero that maps to zero output.
     * @param minOutput  Minimum non-zero output (overcomes static friction).
     * @param curveGain  Controls curve aggressiveness (try 1.0 for linear).
     */
    ExpoDriveCurve(double deadband, double minOutput, double curveGain);
    double curve(double input) override;

private:
    double deadband;
    double minOutput;
    double curveGain;
};

} // namespace vortex
