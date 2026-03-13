#include "Vortex/autonSelector.hpp"
#include "pros/llemu.hpp"
#include "pros/rtos.hpp"

namespace vortex {

static bool selectorInitialized = false;

AutonSelector::AutonSelector() {}

void AutonSelector::addAuton(std::string name, std::function<void()> callback) {
    autons.push_back({std::move(name), std::move(callback)});
}

void AutonSelector::initialize() {
    if (autons.empty() || selectorInitialized) return;
    selectorInitialized = true;

    if (!pros::lcd::is_initialized()) pros::lcd::initialize();
    updateDisplay();

    // Left button cycles backward
    pros::lcd::register_btn0_cb([this]() {
        selectedIndex = (selectedIndex - 1 + (int)autons.size()) % (int)autons.size();
        updateDisplay();
    });

    // Right button cycles forward
    pros::lcd::register_btn2_cb([this]() {
        selectedIndex = (selectedIndex + 1) % (int)autons.size();
        updateDisplay();
    });
}

void AutonSelector::run() {
    if (!autons.empty()) autons[selectedIndex].callback();
}

std::string AutonSelector::getSelectedName() const {
    return autons.empty() ? "None" : autons[selectedIndex].name;
}

void AutonSelector::updateDisplay() {
    if (autons.empty()) {
        pros::lcd::set_text(1, "No autons registered.");
        return;
    }
    pros::lcd::clear();
    pros::lcd::set_text(0, " === Vortex Auton Selector ===");
    pros::lcd::set_text(1, "> " + autons[selectedIndex].name);
    pros::lcd::set_text(2, "  [" + std::to_string(selectedIndex + 1) + "/" +
                            std::to_string(autons.size()) + "]");
    pros::lcd::set_text(4, "  < PREV           NEXT >");
}

} // namespace vortex
