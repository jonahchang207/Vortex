#include "vortex/util/gui/dashboard.hpp"
#include "pros/misc.hpp"
#include "pros/rtos.hpp"
#include <cstdio>

namespace vortex {

static Dashboard* instance = nullptr;

Dashboard::Dashboard(Chassis& chassis, AutonSelector& selector) 
    : chassis(chassis), selector(selector) {
    instance = this;
}

void Dashboard::initialize() {
    lv_obj_t* screen = lv_screen_active();
    lv_obj_clean(screen);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x121212), 0); // Dark theme

    // Split screen: Left for Selector, Right for Telemetry
    lv_obj_t* main_cont = lv_obj_create(screen);
    lv_obj_set_size(main_cont, 480, 240);
    lv_obj_set_style_bg_opa(main_cont, 0, 0);
    lv_obj_set_style_border_width(main_cont, 0, 0);
    lv_obj_set_style_pad_all(main_cont, 0, 0);
    lv_obj_clear_flag(main_cont, LV_OBJ_FLAG_SCROLLABLE);

    create_selector_panel(main_cont);
    create_telemetry_panel(main_cont);

    // Status Bar (Top)
    status_bar = lv_obj_create(screen);
    lv_obj_set_size(status_bar, 480, 30);
    lv_obj_set_style_bg_color(status_bar, lv_color_hex(0x1F1F1F), 0);
    lv_obj_set_style_border_width(status_bar, 0, 0);
    lv_obj_set_style_radius(status_bar, 0, 0);
    lv_obj_align(status_bar, LV_ALIGN_TOP_MID, 0, 0);

    battery_label = lv_label_create(status_bar);
    lv_obj_align(battery_label, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_label_set_text(battery_label, "BAT: 0%");
    lv_obj_set_style_text_color(battery_label, lv_color_hex(0x00FF00), 0);

    radio_label = lv_label_create(status_bar);
    lv_obj_align(radio_label, LV_ALIGN_LEFT_MID, 10, 0);
    lv_label_set_text(radio_label, "RADIO: DISCONNECTED");
    lv_obj_set_style_text_color(radio_label, lv_color_hex(0xFF0000), 0);
}

void Dashboard::create_selector_panel(lv_obj_t* parent) {
    lv_obj_t* panel = lv_obj_create(parent);
    lv_obj_set_size(panel, 240, 210);
    lv_obj_align(panel, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_style_bg_color(panel, lv_color_hex(0x1E1E1E), 0);
    lv_obj_set_style_border_color(panel, lv_color_hex(0x333333), 0);
    lv_obj_set_style_radius(panel, 0, 0);

    // Use the existing AutonSelector logic or replicate here for integration
    // For simplicity, let's add an Alliance toggle and a List
    lv_obj_t* title = lv_label_create(panel);
    lv_label_set_text(title, "AUTONOMOUS SELECTOR");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_12, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, -5);

    lv_obj_t* alliance_btn = lv_button_create(panel);
    lv_obj_set_size(alliance_btn, 200, 35);
    lv_obj_align(alliance_btn, LV_ALIGN_TOP_MID, 0, 15);
    lv_obj_set_style_bg_color(alliance_btn, lv_color_hex(0xD32F2F), 0); // Red
    
    lv_obj_t* btn_label = lv_label_create(alliance_btn);
    lv_label_set_text(btn_label, "ALLIANCE: RED");
    lv_obj_center(btn_label);

    lv_obj_add_event_cb(alliance_btn, [](lv_event_t* e) {
        lv_obj_t* btn = (lv_obj_t*)lv_event_get_target(e);
        lv_obj_t* label = lv_obj_get_child(btn, 0);
        static bool is_red = true;
        is_red = !is_red;
        if (is_red) {
            lv_obj_set_style_bg_color(btn, lv_color_hex(0xD32F2F), 0);
            lv_label_set_text(label, "ALLIANCE: RED");
        } else {
            lv_obj_set_style_bg_color(btn, lv_color_hex(0x1976D2), 0);
            lv_label_set_text(label, "ALLIANCE: BLUE");
        }
    }, LV_EVENT_CLICKED, nullptr);

    // Simple Dropdown for Autons
    lv_obj_t* dd = lv_dropdown_create(panel);
    lv_obj_set_size(dd, 200, 40);
    lv_obj_align(dd, LV_ALIGN_TOP_MID, 0, 60);
    lv_dropdown_clear_options(dd);
    
    // We would ideally populate from 'selector' routines
    // For now, placeholders or integration required
    lv_dropdown_add_option(dd, "Safe WP", 0);
    lv_dropdown_add_option(dd, "Elims Left", 1);
    lv_dropdown_add_option(dd, "Elims Right", 2);
    lv_dropdown_add_option(dd, "Skills", 3);
}

void Dashboard::create_telemetry_panel(lv_obj_t* parent) {
    lv_obj_t* panel = lv_obj_create(parent);
    lv_obj_set_size(panel, 240, 210);
    lv_obj_align(panel, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_set_style_bg_color(panel, lv_color_hex(0x1E1E1E), 0);
    lv_obj_set_style_border_color(panel, lv_color_hex(0x333333), 0);
    lv_obj_set_style_radius(panel, 0, 0);

    lv_obj_t* title = lv_label_create(panel);
    lv_label_set_text(title, "ROBOT TELEMETRY");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_12, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, -5);

    // Pose Group
    lv_obj_t* pose_cont = lv_obj_create(panel);
    lv_obj_set_size(pose_cont, 220, 60);
    lv_obj_align(pose_cont, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_style_bg_color(pose_cont, lv_color_hex(0x2A2A2A), 0);
    lv_obj_set_style_border_width(pose_cont, 1, 0);

    lv_obj_t* p_title = lv_label_create(pose_cont);
    lv_label_set_text(p_title, "CURRENT POSE");
    lv_obj_set_style_text_font(p_title, &lv_font_montserrat_10, 0);
    lv_obj_align(p_title, LV_ALIGN_TOP_LEFT, 5, 2);

    pose_label = lv_label_create(pose_cont);
    lv_label_set_text(pose_label, "X: 0.00  Y: 0.00\nTHETA: 0.00");
    lv_obj_set_style_text_font(pose_label, &lv_font_unscii_8, 0); // Monospaced
    lv_obj_align(pose_label, LV_ALIGN_CENTER, 0, 5);

    // Target Group
    lv_obj_t* target_cont = lv_obj_create(panel);
    lv_obj_set_size(target_cont, 220, 60);
    lv_obj_align(target_cont, LV_ALIGN_TOP_MID, 0, 90);
    lv_obj_set_style_bg_color(target_cont, lv_color_hex(0x2A2A2A), 0);
    lv_obj_set_style_border_width(target_cont, 1, 0);

    lv_obj_t* t_title = lv_label_create(target_cont);
    lv_label_set_text(t_title, "NEXT COORDINATE");
    lv_obj_set_style_text_font(t_title, &lv_font_montserrat_10, 0);
    lv_obj_align(t_title, LV_ALIGN_TOP_LEFT, 5, 2);

    target_label = lv_label_create(target_cont);
    lv_label_set_text(target_label, "X: ---  Y: ---");
    lv_obj_set_style_text_font(target_label, &lv_font_unscii_8, 0);
    lv_obj_align(target_label, LV_ALIGN_CENTER, 0, 5);
}

void Dashboard::update() {
    Pose p = chassis.odom.getPose();
    char buf[64];
    
    // Update Pose
    std::snprintf(buf, sizeof(buf), "X: %.2f  Y: %.2f\nTHETA: %.2f", p.x, p.y, p.theta);
    lv_label_set_text(pose_label, buf);

    // Update Target
    // We'll use NaN or a very large number to indicate no target
    if (std::abs(chassis.getTargetX()) < 10000) {
        std::snprintf(buf, sizeof(buf), "X: %.2f  Y: %.2f", chassis.getTargetX(), chassis.getTargetY());
    } else {
        std::snprintf(buf, sizeof(buf), "X: ---  Y: ---");
    }
    lv_label_set_text(target_label, buf);

    // Update Connectivity
    double batt = pros::battery::get_capacity();
    std::snprintf(buf, sizeof(buf), "BAT: %.0f%%", batt);
    lv_label_set_text(battery_label, buf);
    if (batt < 20) lv_obj_set_style_text_color(battery_label, lv_color_hex(0xFF0000), 0);
    else if (batt < 50) lv_obj_set_style_text_color(battery_label, lv_color_hex(0xFFFF00), 0);
    else lv_obj_set_style_text_color(battery_label, lv_color_hex(0x00FF00), 0);

    bool connected = pros::competition::is_connected();
    lv_label_set_text(radio_label, connected ? "RADIO: CONNECTED" : "RADIO: DISCONNECTED");
    lv_obj_set_style_text_color(radio_label, connected ? lv_color_hex(0x00FF00) : lv_color_hex(0xFF0000), 0);
}

} // namespace vortex
