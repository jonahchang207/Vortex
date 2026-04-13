#include "vortex/util/gui/loading.hpp"
#include "internal.hpp"

namespace vortex {
namespace gui {

void hide_loading_screen() {
    if (loading_scr) {
        lv_obj_del(loading_scr);
        loading_scr = nullptr;
    }
}

} // namespace gui
} // namespace vortex
