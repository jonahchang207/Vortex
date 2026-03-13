#include "Vortex/logger/buffer.hpp"
#include "pros/rtos.hpp"

namespace vortex {

Buffer::Buffer(std::function<void(const std::string&)> handler)
    : handler(handler) {
    // pros::Task has no default constructor — heap-allocate AFTER `this` is valid
    task = new pros::Task([this]() { taskLoop(); });
}

Buffer::~Buffer() {
    running = false;
    pros::delay(rate + 10); // let task exit its current loop iteration
    delete task;
    task = nullptr;
}

void Buffer::push(const std::string& data) {
    mutex.take(TIMEOUT_MAX);
    queue.push_back(data);
    mutex.give();
}

void Buffer::setRate(uint32_t ms) { rate = ms; }

bool Buffer::isEmpty() {
    mutex.take(TIMEOUT_MAX);
    bool empty = queue.empty();
    mutex.give();
    return empty;
}

void Buffer::taskLoop() {
    while (running) {
        std::string item;
        bool hasItem = false;

        mutex.take(TIMEOUT_MAX);
        if (!queue.empty()) {
            item    = queue.front();
            queue.pop_front();
            hasItem = true;
        }
        mutex.give();

        if (hasItem) handler(item);
        pros::delay(rate);
    }
}

} // namespace vortex
