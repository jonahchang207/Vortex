#pragma once
#include <string>
#include <vector>
#include <functional>

namespace vortex {

/**
 * @brief LLEMU-based autonomous routine selector.
 *
 * Shows the routine name on the V5 brain LCD.
 * Left button (btn0) cycles backward, right button (btn2) cycles forward.
 *
 * Usage:
 *   1. addAuton() in initialize() before selector.initialize()
 *   2. selector.initialize() at the end of initialize()
 *   3. selector.run() in autonomous()
 */
class AutonSelector {
public:
    AutonSelector();

    void addAuton(std::string name, std::function<void()> callback);
    void initialize();
    void run();
    std::string getSelectedName() const;

private:
    struct Auton {
        std::string             name;
        std::function<void()>   callback;
    };

    std::vector<Auton> autons;
    int selectedIndex = 0;

    void updateDisplay();
};

} // namespace vortex
