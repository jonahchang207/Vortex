#pragma once
#include <string>
#include <iostream>
#include "pros/rtos.hpp"

namespace vortex {

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR
};

/**
 * @brief Professional Logging and Telemetry system
 */
class Logger {
public:
    static void setLevel(LogLevel level);
    
    /**
     * @brief Log a message with a specific level
     */
    static void log(LogLevel level, const std::string& sender, const std::string& message);

    /**
     * @brief Send telemetry data (name:value) for serial plotting
     */
    static void telemetry(const std::string& name, double value);

private:
    static LogLevel current_level;
};

// Convenience macros
#define V_LOG_DEBUG(sender, msg) vortex::Logger::log(vortex::LogLevel::DEBUG, sender, msg)
#define V_LOG_INFO(sender, msg)  vortex::Logger::log(vortex::LogLevel::INFO, sender, msg)
#define V_LOG_WARN(sender, msg)  vortex::Logger::log(vortex::LogLevel::WARN, sender, msg)
#define V_LOG_ERROR(sender, msg) vortex::Logger::log(vortex::LogLevel::ERROR, sender, msg)

} // namespace vortex
