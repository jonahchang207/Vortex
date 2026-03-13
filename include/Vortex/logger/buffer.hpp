#pragma once
#include <deque>
#include <functional>
#include <string>
#include "pros/rtos.hpp"

namespace vortex {

/**
 * @brief Thread-safe async string queue.
 *
 * Accepts strings from any task, drains them at a fixed rate on a background task.
 * This prevents the main motion loop from blocking on slow I/O (e.g., printf).
 *
 * pros::Task has no default constructor, so it must be heap-allocated via a pointer.
 */
class Buffer {
public:
    explicit Buffer(std::function<void(const std::string&)> handler);
    virtual ~Buffer();

    Buffer(const Buffer&)            = delete;
    Buffer& operator=(const Buffer&) = delete;

    void push(const std::string& data);
    void setRate(uint32_t ms);
    bool isEmpty();

private:
    void taskLoop();

    std::function<void(const std::string&)> handler;
    std::deque<std::string> queue;
    pros::Mutex   mutex;
    pros::Task*   task    = nullptr; // heap-allocated: pros::Task has no default ctor
    uint32_t      rate    = 20;
    bool          running = true;
};

} // namespace vortex
