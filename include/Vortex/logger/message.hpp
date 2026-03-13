#pragma once
#include <string>
#include <cstdint>

namespace vortex {

enum class Level { DEBUG, INFO, WARN, ERROR, FATAL };

struct Message {
    std::string message;
    Level       level;
    uint32_t    time; ///< pros::millis() at creation
};

std::string format_as(Level level);

} // namespace vortex
