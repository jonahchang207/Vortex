#pragma once

#include "api.h"
#include "vortex/vortex.hpp"

#include "autons.hpp"
#include "subsystems.hpp"

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
