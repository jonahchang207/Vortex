#pragma once
#include <cstdint>

namespace vortex {

/**
 * @brief Countdown timer.
 *
 * Uses a start-time snapshot so calling any getter multiple times in one loop
 * does NOT accumulate time (fixes original drift bug).
 */
class Timer {
public:
    /** Create and start a timer. @param time Duration in ms. */
    explicit Timer(uint32_t time);

    uint32_t getTimeSet()    const;
    uint32_t getTimeLeft()   const;
    uint32_t getTimePassed() const;
    bool     isDone()        const;
    bool     isPaused()      const;

    /** Reset and optionally change duration. */
    void set(uint32_t time);
    void reset();

    void pause();
    void resume();

    /** Block until timer expires. */
    void waitUntilDone();

private:
    uint32_t period;
    uint32_t startTime;   ///< millis() at last reset/resume
    uint32_t accumulated; ///< time counted before the most recent pause
    bool     paused;

    /** Current total elapsed time in ms. */
    uint32_t elapsed() const;
};

} // namespace vortex
