# Contributing to Vortex

Thanks for wanting to contribute. The goal is a library that's accurate, readable, and easy to tune — not one that's clever for its own sake.

---

## Setup

1. Fork the repo and clone your fork.
2. Work on a feature branch off `main` — never commit directly to `main` or `stable`.
3. Install dependencies:
   ```bash
   pip install mkdocs-material   # for docs
   brew install gh               # for release automation (macOS)
   ```

---

## Branches

| Branch | Purpose |
|--------|---------|
| `main` | Latest stable-ish code. PRs merge here. |
| `stable` | Last released tag. Docs deploy from here. Never push directly. |
| `feature/*` | Feature work. |
| `fix/*` | Bug fixes. |

---

## Code Standards

- **C++17** — use modern features (`std::optional`, structured bindings, `if constexpr`) where they genuinely improve clarity, not just to be modern.
- **No raw `new`/`delete` in userland code** — use smart pointers or RAII wrappers. The `pros::Task*` in `Buffer` is the only exception and is documented.
- **No global mutable state** — everything lives inside class instances.
- **Every function must have a comment** if its behavior isn't obvious from its name and signature.
- **Units in every variable name or comment** — `distInches`, `timeMs`, `velRpm`. Never leave a bare number without context.
- Format with `.clang-format` before committing:
  ```bash
  clang-format -i src/**/*.cpp include/**/*.hpp
  ```

---

## Math conventions

- Positions in **inches**
- Angles in **radians** internally; degrees at the user-facing API boundary
- Heading: **clockwise from north** (θ=0 = +Y, θ=π/2 = +X)
- Velocities in **motor units (0–127)** at the PID output layer

Any deviation from these must be explicitly documented with a conversion comment.

---

## Testing a change

Vortex doesn't have a hardware-in-the-loop test suite yet. To validate a change:

1. Run `chassis.tuneLateral()` / `chassis.tuneAngular()` and graph the output — settling behavior should be monotonic.
2. Run `chassis.calibratePhysical()` — if the suggested scrubFactor changes by more than 2% from baseline, investigate.
3. Drive a known L-shaped path and measure final position error with a ruler. Should be < 1 inch over 48 inches.

---

## Releasing

Only maintainers release. Use `./vortex-cli` from `main`. See `VORTEX_CLI.md` for the full flow.

---

## Opening a PR

- Keep PRs small and focused — one logical change per PR.
- Add a one-paragraph description of *why* the change is needed, not just *what* it does.
- Reference the bug number or describe the symptom if it's a fix.
- Don't change formatting-only things in the same PR as logic changes.
