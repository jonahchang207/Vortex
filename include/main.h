#ifndef _PROS_MAIN_H_
#define _PROS_MAIN_H_

/**
 * If defined, C++ literals and simplified names are available:
 *   - _rpm, _deg, etc. unit literals
 *   - pros::Motor instead of pros::v5::Motor
 */
#define PROS_USE_SIMPLE_NAMES
#define PROS_USE_LITERALS
#define _PROS_KERNEL_SUPPRESS_LLEMU_WARNING

#include "api.h"
#include "Vortex/api.hpp"

#ifdef __cplusplus
extern "C" {
#endif

void autonomous(void);
void initialize(void);
void disabled(void);
void competition_initialize(void);
void opcontrol(void);

#ifdef __cplusplus
}
#endif

#endif // _PROS_MAIN_H_
