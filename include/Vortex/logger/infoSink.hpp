#pragma once
#include "Vortex/logger/baseSink.hpp"
#include "Vortex/logger/buffer.hpp"

namespace vortex {

// ── Buffered stdout ───────────────────────────────────────────────────────────

/** Writes strings to stdout asynchronously (non-blocking for caller). */
class BufferedStdout : public Buffer {
public:
    BufferedStdout();
    void print(const std::string& msg) { push(msg); }
};

BufferedStdout& bufferedStdout();

// ── Info sink ─────────────────────────────────────────────────────────────────

/** Sink that writes color-coded messages to the PROS terminal. */
class InfoSink : public BaseSink {
public:
    InfoSink();
protected:
    void sendMessage(const Message& msg) override;
};

std::shared_ptr<InfoSink> infoSink();

} // namespace vortex
