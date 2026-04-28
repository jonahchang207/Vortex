#pragma once
#include <string>
#include <vector>
#include "vortex/util/pose.hpp"

namespace vortex {

/**
 * @brief Represents a path file asset
 */
struct Asset {
    const uint8_t* data;
    size_t size;
    
    Asset(const uint8_t* data, size_t size) : data(data), size(size) {}

    // Placeholders for Pure Pursuit
    Pose getPoint(int index) { return {0, 0, 0}; }
    Pose lastPoint() { return {0, 0, 0}; }
};

/**
 * @brief Helper macro to define assets (simulating LemLib's ASSET)
 */
#define VORTEX_ASSET(name) \
    extern "C" uint8_t _binary_static_##name##_start[]; \
    extern "C" uint8_t _binary_static_##name##_size[]; \
    vortex::Asset name(_binary_static_##name##_start, (size_t)_binary_static_##name##_size);

} // namespace vortex
