#include "vortex/util/gui/loading.hpp"
#include "internal.hpp"
#include "pros/misc.hpp"

namespace vortex {
namespace gui {

void show_loading_error() {
    if (progress_bar) {
        // Red Pulse Animation
        lv_obj_set_style_bg_color(progress_bar, lv_palette_main(LV_PALETTE_RED), LV_PART_INDICATOR);
        
        // Simple pulsing effect using styles or a loop in the caller
        // Here we just set it to red as a state change
    }

    // Continuous rumble
    pros::Controller master(pros::E_CONTROLLER_MASTER);
    master.rumble("-"); // Long pulse for error
}

} // namespace gui
} // namespace vortex
