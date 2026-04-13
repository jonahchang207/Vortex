#include "vortex/util/gui/loading.hpp"
#include "internal.hpp"

namespace vortex {
namespace gui {

void update_loading_bar(int percent) {
    if (progress_bar) {
        lv_bar_set_value(progress_bar, percent, LV_ANIM_ON);
    }
}

} // namespace gui
} // namespace vortex
