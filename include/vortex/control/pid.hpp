#pragma once
#include "vortex/util/exitcondition.hpp"
#include <algorithm>

namespace vortex {

struct PIDSettings {
    double kP;
    double kI;
    double kD;
    double windup_range = 0; // Anti-windup range (0 to disable)
    double slew = 0;         // Max acceleration (0 to disable)
};

class PID {
public:
    PID(double kP, double kI, double kD);
    PID(PIDSettings settings, ExitCondition exit_condition = {0, 0, 0});

    /**
     * @brief Updates the PID controller with a new error and returns the output
     */
    double update(double error);

    /**
     * @brief Resets the internal state (integral, prev_error, slew)
     */
    void reset();

    /**
     * @brief Checks if the PID has settled based on its exit condition
     */
    bool isSettled(double error);

    void setSettings(PIDSettings settings);
    void setExitCondition(ExitCondition exit_condition);

private:
    PIDSettings settings;
    ExitCondition exit;
    
    double integral = 0;
    double prev_error = 0;
    double prev_output = 0;
};

} // namespace vortex
