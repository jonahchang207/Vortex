#pragma once

namespace vortex {

/**
 * @brief Discrete PID controller.
 *
 * Output = kP*e + kI*∫e + kD*(Δe/Δt)
 * where the "time" unit is one call to update().
 */
class PID {
public:
    struct Constants {
        double kP           = 0;
        double kI           = 0;
        double kD           = 0;
        double windupRange  = 0;    ///< integral only accumulates when |error| < this
        bool   signFlipReset = false; ///< reset integral when error crosses zero
    };

    PID(double kP, double kI, double kD, double windupRange = 0, bool signFlipReset = false);
    explicit PID(Constants c);

    /**
     * @brief Feed in current error, get motor power output.
     * Call once per loop iteration (10 ms typical).
     */
    double update(double error);

    /** Clear integral and previous-error state. Call before every new motion. */
    void reset();

    Constants getConstants() const { return constants; }
    void setConstants(Constants c)  { constants = c; }

private:
    Constants constants;
    double integral  = 0;
    double prevError = 0;
};

} // namespace vortex
