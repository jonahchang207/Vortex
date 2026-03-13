#include "Vortex/timer.hpp"
#include "pros/rtos.hpp"

namespace vortex {

Timer::Timer(uint32_t time)
    : period(time), startTime(pros::millis()), accumulated(0), paused(false) {}

// Returns total ms elapsed, accounting for any paused intervals.
// Calling this multiple times in the same ms does NOT accumulate extra time.
uint32_t Timer::elapsed() const {
    if (paused) return accumulated;
    return accumulated + (pros::millis() - startTime);
}

uint32_t Timer::getTimeSet()    const { return period; }
uint32_t Timer::getTimeLeft()   const { uint32_t e = elapsed(); return (e < period) ? period - e : 0; }
uint32_t Timer::getTimePassed() const { return elapsed(); }

bool Timer::isDone()   const { return elapsed() >= period; }
bool Timer::isPaused() const { return paused; }

void Timer::set(uint32_t time) {
    period = time;
    reset();
}

void Timer::reset() {
    accumulated = 0;
    startTime   = pros::millis();
    // paused state is preserved — if you were paused you stay paused
}

void Timer::pause() {
    if (!paused) {
        accumulated += pros::millis() - startTime; // bank elapsed time
        paused = true;
    }
}

void Timer::resume() {
    if (paused) {
        startTime = pros::millis(); // fresh reference point
        paused    = false;
    }
}

void Timer::waitUntilDone() {
    while (!isDone()) pros::delay(5);
}

} // namespace vortex
