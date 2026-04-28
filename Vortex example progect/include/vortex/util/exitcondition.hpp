#pragma once
#include "pros/rtos.hpp"

namespace vortex {

/**
 * @brief Class to manage exit conditions for movements
 */
class ExitCondition {
public:
    /**
     * @brief Construct a new Exit Condition object
     * 
     * @param range The error range that counts as "settled"
     * @param settle_time The amount of time (ms) the error must stay within range to exit
     * @param timeout The maximum amount of time (ms) the movement can run
     */
    ExitCondition(double range, int settle_time, int timeout)
        : range(range), settle_time(settle_time), timeout(timeout) {}

    /**
     * @brief Update the exit condition with the latest error
     * 
     * @param error Latest error value
     * @return true if the movement should exit
     * @return false if the movement should continue
     */
    bool update(double error) {
        uint32_t now = pros::millis();
        if (start_time == 0) start_time = now;

        // Check timeout
        if (now - start_time > timeout) return true;

        // Check settling
        if (std::abs(error) < range) {
            if (settle_start_time == 0) settle_start_time = now;
            if (now - settle_start_time > settle_time) return true;
        } else {
            settle_start_time = 0;
        }

        return false;
    }

    /**
     * @brief Resets the internal timers
     */
    void reset() {
        start_time = 0;
        settle_start_time = 0;
    }

private:
    double range;
    int settle_time;
    int timeout;

    uint32_t start_time = 0;
    uint32_t settle_start_time = 0;
};

} // namespace vortex
