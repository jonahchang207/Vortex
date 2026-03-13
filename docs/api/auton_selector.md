# AutonSelector

`#include "Vortex/autonSelector.hpp"`

LLEMU-based autonomous routine selector. Left button cycles backward, right cycles forward.

---

## Usage

```cpp
vortex::AutonSelector selector;

void blueSafe() { /* ... */ }
void redSafe()  { /* ... */ }
void skills()   { /* ... */ }

void initialize() {
    chassis.calibrate();

    selector.addAuton("Blue Safe", blueSafe);
    selector.addAuton("Red Safe",  redSafe);
    selector.addAuton("Skills",    skills);
    selector.initialize(); // registers LLEMU button callbacks
}

void autonomous() {
    selector.run(); // calls whichever routine is selected
}
```

---

## Methods

### `addAuton(name, callback)`
Register a routine. Can be any `std::function<void()>` — lambda or function pointer.

### `initialize()`
Sets up LLEMU display and button callbacks. Call at the end of `initialize()`. Only the first call has effect — safe to call from multiple places.

### `run()`
Execute the currently selected routine. Call from `autonomous()`.

### `getSelectedName()`
Returns the name string of the currently selected routine.

---

## Display

The brain LCD shows:
```
=== Vortex Auton Selector ===
> Blue Safe
  [1/3]

  < PREV           NEXT >
```
