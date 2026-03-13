#include "Vortex/logger/infoSink.hpp"
#include "Vortex/logger/message.hpp"
#include <iostream>

namespace vortex {

// ── BaseSink ─────────────────────────────────────────────────────────────────

void BaseSink::setLowestLevel(Level level) {
    if (!sinks.empty())
        for (auto& s : sinks) s->setLowestLevel(level);
    else
        lowestLevel = level;
}

void BaseSink::log(Level level, const std::string& message) {
    if (!sinks.empty()) {
        for (auto& s : sinks) s->log(level, message);
        return;
    }
    if (level < lowestLevel) return;
    sendMessage({message, level, pros::millis()});
}

// ── BufferedStdout ────────────────────────────────────────────────────────────

BufferedStdout::BufferedStdout()
    : Buffer([](const std::string& s) { std::cout << s << std::flush; }) {
    setRate(50);
}

BufferedStdout& bufferedStdout() {
    static BufferedStdout instance;
    return instance;
}

// ── InfoSink ─────────────────────────────────────────────────────────────────

static const char* levelColor(Level l) {
    switch (l) {
        case Level::DEBUG: return "\033[0;36m";   // cyan
        case Level::INFO:  return "\033[0;32m";   // green
        case Level::WARN:  return "\033[0;33m";   // yellow
        case Level::ERROR: return "\033[0;31m";   // red
        case Level::FATAL: return "\033[1;31m";   // bold red
    }
    return "";
}

InfoSink::InfoSink() { lowestLevel = Level::INFO; }

void InfoSink::sendMessage(const Message& msg) {
    std::string out = "[Vortex] ";
    out += levelColor(msg.level);
    out += format_as(msg.level);
    out += ": ";
    out += msg.message;
    out += "\033[0m\n";
    bufferedStdout().print(out);
}

std::shared_ptr<InfoSink> infoSink() {
    static std::shared_ptr<InfoSink> sink = std::make_shared<InfoSink>();
    return sink;
}

} // namespace vortex
