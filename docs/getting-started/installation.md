# Installation

Vortex is a template for the **PROS** environment for VEX V5.

## Prerequisites
1.  **PROS Kernel**: Ensure you have [PROS installed](https://pros.cs.purdue.edu/).
2.  **CLI**: Familiarity with the `prosv5` command line or the PROS VSCode extension.

---

## Setting up Vortex

### 1. Create a New Project
Create a standard PROS project:
```bash
prosv5 conduct create ./my-vortex-project
```

### 2. Include Vortex
Download the latest Vortex release and copy the `include/vortex` and `src/vortex` folders into your project.

### 3. Setup Include Paths
In your `include/main.h`, add:
```cpp
#include "vortex/vortex.hpp"
```

---

## Template Structure

*   **vortex/drive**: Odometry and Chassis control logic.
*   **vortex/control**: PID and Velocity Profiles.
*   **vortex/util**: Math, Logging, and GUI utilities.
*   **vortex/vortex.hpp**: The "umbrella" header.
