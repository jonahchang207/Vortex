#pragma once
#include "vortex/drive/chassis.hpp"
#include "vortex/util/auton_selector.hpp"
#include "liblvgl/lvgl.h"

namespace vortex {

/**
 * @brief Dashboard for displaying robot status and selecting autonomous
 */
class Dashboard {
public:
    /**
     * @brief Construct a new Dashboard object
     * 
     * @param chassis Reference to the robot chassis
     * @param selector Reference to the autonomous selector
     */
    Dashboard(Chassis& chassis, AutonSelector& selector);

    /**
     * @brief Initialize the dashboard UI
     */
    void initialize();

    /**
     * @brief Update the dashboard data
     */
    void update();

private:
    Chassis& chassis;
    AutonSelector& selector;

    // UI Elements
    lv_obj_t* pose_label;
    lv_obj_t* target_label;
    lv_obj_t* battery_label;
    lv_obj_t* radio_label;
    lv_obj_t* status_bar;

    void create_telemetry_panel(lv_obj_t* parent);
    void create_selector_panel(lv_obj_t* parent);
};

} // namespace vortex
