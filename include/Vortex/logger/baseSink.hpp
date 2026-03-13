#pragma once
#include <memory>
#include <vector>
#include <initializer_list>
#include "Vortex/logger/message.hpp"
#include "pros/rtos.hpp"

namespace vortex {

/** Abstract logging sink. Subclass and override sendMessage(). */
class BaseSink {
public:
    BaseSink() = default;
    BaseSink(std::initializer_list<std::shared_ptr<BaseSink>> sinks) : sinks(sinks) {}
    virtual ~BaseSink() = default;

    void setLowestLevel(Level level);
    virtual void log(Level level, const std::string& message);

    // Convenience wrappers
    void debug(const std::string& m) { log(Level::DEBUG, m); }
    void info (const std::string& m) { log(Level::INFO,  m); }
    void warn (const std::string& m) { log(Level::WARN,  m); }
    void error(const std::string& m) { log(Level::ERROR, m); }
    void fatal(const std::string& m) { log(Level::FATAL, m); }

protected:
    virtual void sendMessage(const Message&) {}

    Level lowestLevel = Level::INFO;
    std::vector<std::shared_ptr<BaseSink>> sinks;
};

} // namespace vortex
