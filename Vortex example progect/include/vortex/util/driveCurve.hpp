#pragma once
#include <cmath>
#include <algorithm>

namespace vortex {

/**
 * @brief Base class for drive curves (joystick scaling)
 */
class DriveCurve {
public:
    virtual ~DriveCurve() = default;
    virtual double calculate(double input) = 0;
};

/**
 * @brief Linear drive curve with optional deadband
 */
class LinearDriveCurve : public DriveCurve {
public:
    LinearDriveCurve(double deadband = 2.0) : deadband(deadband) {}
    
    double calculate(double input) override {
        if (std::abs(input) < deadband) return 0;
        return input;
    }

private:
    double deadband;
};

/**
 * @brief Exponential drive curve for smoother low-speed control
 */
class ExpoDriveCurve : public DriveCurve {
public:
    /**
     * @brief Construct a new Expo Drive Curve object
     * 
     * @param deadband Inputs below this value will be ignored
     * @param min_output Minimum power output once past deadband
     * @param curve Curve factor. Higher = more sensitive at high stick values, less at low
     */
    ExpoDriveCurve(double deadband, double min_output, double curve)
        : deadband(deadband), min_output(min_output), curve(curve) {}

    double calculate(double input) override {
        if (std::abs(input) < deadband) return 0;
        
        // Normalize input to 0-1
        double x = std::abs(input) / 127.0;
        
        // Apply expo formula
        double output = min_output + (127.0 - min_output) * (std::pow(curve, x) - 1.0) / (curve - 1.0);
        
        return (input > 0) ? output : -output;
    }

private:
    double deadband;
    double min_output;
    double curve;
};

} // namespace vortex
