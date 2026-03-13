#include "Vortex/driveCurve.hpp"
#include "Vortex/util.hpp"
#include <cmath>

namespace vortex {

ExpoDriveCurve::ExpoDriveCurve(double deadband, double minOutput, double curveGain)
    : deadband(deadband), minOutput(minOutput), curveGain(curveGain) {}

double ExpoDriveCurve::curve(double input) {
    if (std::abs(input) <= deadband) return 0.0;

    // Map input outside deadband to [0, 127]
    double g    = std::abs(input) - deadband;
    double g127 = 127.0 - deadband;

    // Exponential scaling: base^(g-127) grows from near-0 to 1 as g→127
    double i    = std::pow(curveGain, g    - 127.0) * g;
    double i127 = std::pow(curveGain, g127 - 127.0) * g127;

    // Scale to [minOutput, 127] and reapply sign
    return (127.0 - minOutput) / 127.0 * (i / i127) * 127.0 * sgn(input)
         + minOutput * sgn(input);
}

} // namespace vortex
