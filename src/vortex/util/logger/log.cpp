#include "vortex/util/logger.hpp"
#include <iostream>

namespace vortex {

LogLevel Logger::current_level = LogLevel::INFO;

void Logger::log(LogLevel level, const std::string& sender, const std::string& msg) {
    if (level < current_level) return;
    
    std::string level_str;
    switch(level) {
        case LogLevel::INFO: level_str = "[INFO]"; break;
        case LogLevel::DEBUG: level_str = "[DEBUG]"; break;
        case LogLevel::WARN: level_str = "[WARN]"; break;
        case LogLevel::ERROR: level_str = "[ERR]"; break;
    }
    
    std::cout << level_str << " (" << sender << "): " << msg << std::endl;
}

void Logger::telemetry(const std::string& name, double value) {
    // Format for common serial plotters: ">Name:Value"
    std::cout << ">" << name << ":" << value << std::endl;
}

} // namespace vortex
