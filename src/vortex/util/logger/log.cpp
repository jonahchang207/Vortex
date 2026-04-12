#include "vortex/util/logger.hpp"
#include <iostream>

namespace vortex {

void Logger::log(Level level, std::string sender, std::string msg) {
    if (level < current_level) return;
    
    std::string level_str;
    switch(level) {
        case Level::INFO: level_str = "[INFO]"; break;
        case Level::DEBUG: level_str = "[DEBUG]"; break;
        case Level::WARN: level_str = "[WARN]"; break;
        case Level::ERR: level_str = "[ERR]"; break;
    }
    
    std::cout << level_str << " (" << sender << "): " << msg << std::endl;
}

void Logger::telemetry(std::string name, double value) {
    // Format for common serial plotters: ">Name:Value"
    std::cout << ">" << name << ":" << value << std::endl;
}

} // namespace vortex
