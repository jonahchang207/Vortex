# Installation

## Prerequisites

- PROS 4.x or higher
- A VEX V5 Brain
- VS Code with the PROS extension (recommended)

---

## Method 1 — Manual ZIP (Recommended)

This method bypasses common PROS CLI kernel compatibility errors.

1. Download `Vortex.zip` from the [latest GitHub release](https://github.com/jonahchang207/Vortex/releases/latest).
2. Move `Vortex.zip` into the root of your PROS project.
3. Run in your project terminal:

```bash
pros c fetch Vortex.zip
pros c install Vortex
```

---

## Method 2 — Direct CLI

```bash
pros c fetch https://github.com/jonahchang207/Vortex/releases/download/v2.0.6/Vortex.zip
pros c install Vortex@2.0.6
```

If you see `does not support kernel version`, use Method 1 instead.

---

## Upgrading

```bash
pros c upgrade Vortex
```

---

## Verify

Add to `main.h`:

```cpp
#include "Vortex/api.hpp"
```

Then build:

```bash
make
```

A clean build with no errors means Vortex is installed correctly.
