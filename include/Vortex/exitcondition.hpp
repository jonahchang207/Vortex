#pragma once
#include <cstdint>

namespace vortex {

/**
 * @brief Decides when a PID motion is "done."
 *
 * The robot must stay within `range` of the target for `settleTime` ms.
 * There is also a hard `maxTime` timeout from the first call to update(),
 * which prevents the robot from getting stuck forever.
 */
class ExitCondition {
public:
    /**
     * @param range       Error magnitude considered "at target."
     * @param settleTime  How long (ms) the robot must stay within range.
     * @param maxTime     Hard timeout (ms) from motion start; 0 = disabled.
     */
    ExitCondition(double range, int settleTime, int maxTime);

    /** Feed current error each loop; call getExit() after to check. */
    void update(double error);

    /** Returns true when the settle condition OR the hard timeout is met. */
    bool getExit() const;

    /** Reset all timers — call this before each new motion. */
    void reset();

private:
    double   range;
    int      settleTime;
    int      maxTime;
    uint32_t startTime   = 0; ///< time of first update() call
    uint32_t firstInTime = 0; ///< time error first entered range
};

} // namespace vortex
