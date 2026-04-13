#include "vortex/util/gui/loading.hpp"
#include "internal.hpp"

namespace vortex {
namespace gui {

lv_obj_t* loading_scr = nullptr;
lv_obj_t* logo_img = nullptr;
lv_obj_t* progress_bar = nullptr;

void show_loading_screen() {
    loading_scr = lv_obj_create(nullptr);
    lv_scr_load(loading_scr);

    // Create Logo (Placeholder for the generated image)
    // In a real project, the user would convert 'vortex_logo_npr.png' to a C array
    logo_img = lv_img_create(loading_scr);
    lv_obj_center(logo_img);
    lv_obj_set_y(logo_img, -20);

    // Create Loading Bar
    progress_bar = lv_bar_create(loading_scr);
    lv_obj_set_size(progress_bar, 300, 20);
    lv_obj_align(progress_bar, LV_ALIGN_CENTER, 0, 80);
    lv_bar_set_range(progress_bar, 0, 100);
    lv_bar_set_value(progress_bar, 0, LV_ANIM_OFF);
    
    // Aesthetic Styling
    lv_obj_set_style_bg_color(progress_bar, lv_palette_main(LV_PALETTE_TEAL), LV_PART_INDICATOR);
}

} // namespace gui
} // namespace vortex
