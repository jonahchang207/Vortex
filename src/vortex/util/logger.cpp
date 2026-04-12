#include "vortex/util/logger.hpp"
#include <iomanip>

namespace vortex {

LogLevel Logger::current_level = LogLevel::INFO;

void Logger::setLevel(LogLevel level) {
    current_level = level;
}

void Logger::log(LogLevel level, const std::string& sender, const std::string& message) {
    if (level < current_level) return;

    std::string level_str;
    switch (level) {
        case LogLevel::DEBUG: level_str = "DEBUG"; break;
        case LogLevel::INFO:  level_str = "INFO "; break;
        case LogLevel::WARN:  level_str = "WARN "; break;
        case LogLevel::ERROR: level_str = "ERROR"; break;
    }

    uint32_t mil = pros::millis();
    std::cout << "[" << std::setw(6) << mil << "] "
              << "[" << level_str << "] "
              << "[" << sender << "] "
              << message << std::endl;
}

void Logger::telemetry(const std::string& name, double value) {
    // Format compatible with many serial plotters: "name:value" or "name=value"
    std::cout << ">" << name << ":" << value << std::endl;
}

} // namespace vortex
