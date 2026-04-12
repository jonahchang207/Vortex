#pragma once

namespace vortex {

struct PIDConstants {
    double kP;
    double kI;
    double kD;
};

class PID {
public:
    PID(double kP, double kI, double kD);
    PID(PIDConstants constants);

    /**
     * @brief Updates the PID controller with a new error and returns the output
     */
    double update(double error);

    /**
     * @brief Resets the internal state (integral, prev_error)
     */
    void reset();

    void setConstants(PIDConstants constants);

private:
    double kP, kI, kD;
    double integral = 0;
    double prev_error = 0;
};

} // namespace vortex
