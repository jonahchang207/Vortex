#pragma once
#include <cstddef>
#include <cstdint>

#ifndef _VORTEX_ASSET_H_
#define _VORTEX_ASSET_H_

extern "C" {
/** Raw binary asset embedded in the ELF by the linker. */
typedef struct __attribute__((__packed__)) _vortex_asset {
    uint8_t* buf;
    size_t   size;
} asset;
}

/**
 * @brief Declare an embedded path asset.
 *
 * Place your CSV path files in static/ and reference them with their
 * filename (replacing dots with underscores).
 *
 * Example: ASSET(my_path_txt) for static/my_path.txt
 */
#define ASSET(x)                                                                 \
    extern "C" {                                                                 \
    extern uint8_t _binary_static_##x##_start[], _binary_static_##x##_size[];   \
    static asset x = {_binary_static_##x##_start,                               \
                      (size_t)_binary_static_##x##_size};                        \
    }

#endif // _VORTEX_ASSET_H_
