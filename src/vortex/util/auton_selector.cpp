#include "vortex/util/auton_selector.hpp"
#include "pros/screen.h"
#include "liblvgl/lvgl.h"

namespace vortex {

static AutonSelector* current_selector = nullptr;

AutonSelector::AutonSelector(std::vector<Auton> routines) : routines(routines) {
    current_selector = this;
}

void AutonSelector::alliance_toggle_handler(lv_event_t* e) {
    lv_obj_t* btn = (lv_obj_t*)lv_event_get_target(e);
    if (!current_selector) return;

    if (current_selector->selected_alliance == Alliance::RED) {
        current_selector->selected_alliance = Alliance::BLUE;
        lv_obj_set_style_bg_color(btn, lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_label_set_text(lv_obj_get_child(btn, 0), "Alliance: BLUE");
    } else {
        current_selector->selected_alliance = Alliance::RED;
        lv_obj_set_style_bg_color(btn, lv_palette_main(LV_PALETTE_RED), 0);
        lv_label_set_text(lv_obj_get_child(btn, 0), "Alliance: RED");
    }
}

void AutonSelector::initialize() {
    // Parent object for the whole screen
    lv_obj_t* screen = lv_screen_active();
    lv_obj_clean(screen);

    // Alliance Toggle Button
    lv_obj_t* alliance_btn = lv_button_create(screen);
    lv_obj_set_size(alliance_btn, 160, 40);
    lv_obj_align(alliance_btn, LV_ALIGN_TOP_RIGHT, -10, 10);
    lv_obj_set_style_bg_color(alliance_btn, lv_palette_main(LV_PALETTE_RED), 0);
    
    lv_obj_t* btn_label = lv_label_create(alliance_btn);
    lv_label_set_text(btn_label, "Alliance: RED");
    lv_obj_center(btn_label);
    
    lv_obj_add_event_cb(alliance_btn, alliance_toggle_handler, LV_EVENT_CLICKED, nullptr);

    // Routine Tabs
    lv_obj_t* tv = lv_tabview_create(screen);
    lv_tabview_set_tab_bar_position(tv, LV_DIR_LEFT);
    lv_tabview_set_tab_bar_size(tv, 100);
    lv_obj_set_size(tv, 480, 240);
    lv_obj_align(tv, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    
    for (size_t i = 0; i < routines.size(); ++i) {
        lv_obj_t* tab = lv_tabview_add_tab(tv, routines[i].name.c_str());
        
        lv_obj_t* label = lv_label_create(tab);
        lv_label_set_text(label, routines[i].description.c_str());
        lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, 10, 10);

        lv_obj_set_user_data(tab, (void*)i);
    }

    lv_obj_add_event_cb(tv, [](lv_event_t* e) {
        lv_obj_t* tv = (lv_obj_t*)lv_event_get_target(e);
        if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
            if (current_selector) {
                current_selector->selected_index = lv_tabview_get_tab_active(tv);
            }
        }
    }, LV_EVENT_VALUE_CHANGED, nullptr);
}

void AutonSelector::run() {
    if (selected_index >= 0 && selected_index < routines.size()) {
        routines[selected_index].callback();
    }
}

std::string AutonSelector::getSelectedName() const {
    if (selected_index >= 0 && selected_index < routines.size()) {
        return routines[selected_index].name;
    }
    return "None";
}

Alliance AutonSelector::getAlliance() const {
    return selected_alliance;
}

} // namespace vortex
