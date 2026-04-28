#pragma once
#include <cstdint>
#include "pros/rtos.hpp"

namespace vortex {

/**
 * @brief Non-blocking timer utility
 */
class Timer {
public:
    Timer() : start_time(pros::millis()) {}
    
    /**
     * @brief Reset the timer to now
     */
    void reset() {
        start_time = pros::millis();
    }

    /**
     * @brief Get elapsed time in milliseconds
     */
    uint32_t getElapsed() const {
        return pros::millis() - start_time;
    }

    /**
     * @brief Check if a target duration has passed
     * @param duration Duration in milliseconds
     */
    bool hasPassed(uint32_t duration) const {
        return getElapsed() >= duration;
    }

    /**
     * @brief Blocks execution until the timer reaches the duration
     * @param duration Target time in milliseconds
     */
    void waitUntil(uint32_t duration) {
        while (!hasPassed(duration)) {
            pros::delay(10);
        }
    }

private:
    uint32_t start_time;
};

} // namespace vortex
