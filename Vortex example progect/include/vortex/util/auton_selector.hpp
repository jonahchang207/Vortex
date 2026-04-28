#pragma once
#include <string>
#include <vector>
#include <functional>
#include "liblvgl/lvgl.h"

namespace vortex {

/**
 * @brief Represents a single autonomous routine
 */
struct Auton {
    std::string name;
    std::string description;
    std::function<void()> callback;
};

enum class Alliance {
    RED,
    BLUE
};

/**
 * @brief Manages selection of autonomous routines via LVGL
 */
class AutonSelector {
public:
    AutonSelector(std::vector<Auton> routines);

    /**
     * @brief Creates the LVGL interface on the brain screen
     */
    void initialize();

    /**
     * @brief Runs the selected autonomous routine
     */
    void run();

    /**
     * @brief Gets the name of the currently selected routine
     */
    std::string getSelectedName() const;

    /**
     * @brief Gets the currently selected alliance
     */
    Alliance getAlliance() const;

private:
    std::vector<Auton> routines;
    int selected_index = 0;
    Alliance selected_alliance = Alliance::RED;
    
    // LVGL internal callbacks (must be static)
    static void alliance_toggle_handler(lv_event_t* e);
};

} // namespace vortex
